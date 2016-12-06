#include <cstdlib>
#include <initializer_list>

#include "s7/s7.h"

#include "graph/interpreter.hpp"
#include "graph/sheet.hpp"
#include "graph/instance.hpp"
#include "graph/cell.hpp"
#include "graph/dependencies.hpp"

namespace Graph
{
////////////////////////////////////////////////////////////////////////////////
// Helper functions
static s7_pointer encode_cell_key(s7_scheme* interpreter, const CellKey& k)
{
    auto out = s7_nil(interpreter);
    for (auto& i : k.first)
    {
        out = s7_cons(interpreter, s7_make_c_pointer(interpreter, i), out);
    }
    out = s7_cons(interpreter, s7_make_c_pointer(interpreter, k.second), out);
    return out;
}

static s7_pointer encode_name_key(s7_scheme* interpreter, const NameKey& k)
{
    auto out = s7_nil(interpreter);
    for (auto& i : k.first)
    {
        out = s7_cons(interpreter, s7_make_c_pointer(interpreter, i), out);
    }
    out = s7_cons(interpreter, s7_make_string(interpreter, k.second.c_str()), out);
    return out;
}

static CellKey decode_cell_key(s7_scheme* interpreter, s7_pointer v)
{
    CellKey out;
    out.second = static_cast<Cell*>(s7_c_pointer(s7_car(v)));
    v = s7_cdr(v);

    // Because of the list's order, we fill the key from back to front
    out.first.resize(s7_list_length(interpreter, v));
    for (auto itr = out.first.rbegin(); itr != out.first.rend();
         ++itr, v = s7_cdr(v))
    {
        *itr = static_cast<Instance*>(s7_c_pointer(s7_car(v)));
    }

    return out;
}

static NameKey decode_name_key(s7_scheme* interpreter, s7_pointer v)
{
    NameKey out;
    out.second = std::string(s7_string(s7_car(v)));
    v = s7_cdr(v);

    // Because of the list's order, we fill the key from back to front
    out.first.resize(s7_list_length(interpreter, v));
    for (auto itr = out.first.rbegin(); itr != out.first.rend();
         ++itr, v = s7_cdr(v))
    {
        *itr = static_cast<Instance*>(s7_c_pointer(s7_car(v)));
    }

    return out;
}

static s7_pointer check_upstream_(s7_scheme* interpreter, s7_pointer args)
{
    auto& deps = *static_cast<Dependencies*>(s7_c_pointer(s7_car(args)));
    auto lookee = decode_name_key(interpreter, s7_cadr(args));
    auto looker = decode_cell_key(interpreter, s7_caddr(args));

    // Success
    int out = 0;

    if (deps.insert(looker, lookee))
    {
        // Failure due to recursive lookup
        out = 1;
    }
    else if (isCellKey(lookee) &&
             toCellKey(lookee).second->values.count(lookee.first) == 0)
    {
        // Failure due to non-existent value
        out = -1;
    }
    return s7_make_integer(interpreter, out);
}

////////////////////////////////////////////////////////////////////////////////

Interpreter::Interpreter()
    : interpreter(s7_init()),
      check_upstream(s7_make_function(
                interpreter, "check-upstream", check_upstream_,
                3, /* required args */
                0, /* optional args */
                false, /* rest args */
                "(check-upstream deps target looker)")),
      value_thunk_factory(s7_eval_c_string(interpreter,R"(
           (lambda (deps target looker check-upstream value)
               (lambda ()
               (let ((res (check-upstream deps target looker)))
                   (cond ((=  1 res) (error 'circular-lookup "Circular lookup"))
                         ((= -1 res) (error 'no-such-value "Missing value"))
                         ((eqv? 'value (car value)) (cdr value))
                         (else (error 'invalid-lookup "Invalid lookup"))))))
          )")),
      instance_thunk_factory(s7_eval_c_string(interpreter, R"(
          (lambda (deps target looker check-upstream values)
            (let ((lookup (apply hash-table values))
                  (keys (apply hash-table
                    (map (lambda (v) (cons (car v) #t)) values))))
            (lambda (key)
              (let ((res (check-upstream deps target looker)))
                (cond  ((=  1 res) (error 'circular-lookup "Circular lookup"))
                       ((not (hash-table-ref keys key))
                         (error 'missing-lookup "~A: missing instance lookup in ~A" key (car target)))
                       (else ((hash-table-ref lookup key))))))))
      )")),
      eval_func(s7_eval_c_string(interpreter, R"(
        (lambda (str env)
          (define (read-all port)
            (let recurse ()
              (let ((r (read port)))
                (if (eq? r #<eof>)
                  '()
                  (cons r (recurse))))))
          (define (strip-output expr)
            (if (and (> (length expr) 0)
                     (list? (car expr))
                     (> (length (car expr)) 1)
                     (eq? 'output (caar expr)))
              (if (> (length expr) 1)
                (error "Output must only have one clause")
                (cdar expr))
              expr))
          (catch #t
            (lambda ()
              (let* ((i (read-all (open-input-string str)))
                     (j (strip-output i))
                     (r (cons 'begin j)))
                (cons 'value (eval r env))))
            (lambda args
              (cond ((string=? "~A: unbound variable" (caadr args))
                        (copy (cons 'unbound-var (cadr args))))
                    ((string=? "~A: missing instance lookup in ~A" (caadr args))
                        (copy (cons 'unbound-instance (cadr args))))
                    (else
                        (copy (cons 'error (cadr args))))))))
      )")),
      is_input(s7_eval_c_string(interpreter, R"(
        (lambda (str)
          (catch #t
            (lambda ()
              (let ((r (read (open-input-string str))))
                (eqv? (car r) 'input)))
            (lambda args #f)))
      )")),
      is_output(s7_eval_c_string(interpreter, R"(
        (lambda (str)
          (catch #t
            (lambda ()
              (let ((r (read (open-input-string str))))
                (eqv? (car r) 'output)))
            (lambda args #f)))
      )")),
      default_expr(s7_eval_c_string(interpreter, R"(
        (lambda (str)
          (catch #t
            (lambda ()
              (let ((r (read (open-input-string str))))
                (if (eqv? (car r) 'input)
                    (cadr r)
                    "")))
            (lambda args "")))
      )")),
      name_valid(s7_eval_c_string(interpreter, R"(
        (lambda (str)
          (and (not (char-position #\( str))
               (not (char-position #\) str))
               (not (char-position #\  str))
              (catch #t
                (lambda ()
                  (let* ((s (format #f "(define ~a #t) " str))
                         (r (read (open-input-string s))))
                    (eval r)
                    #t))
                (lambda args #f))))
      )"))
{
    for (s7_pointer ptr: {check_upstream,  value_thunk_factory,
                          eval_func, is_input, is_output, default_expr})
    {
        s7_gc_protect(interpreter, ptr);
    }
}

Interpreter::~Interpreter()
{
    free(interpreter);
}

void Interpreter::release(Cell* cell)
{
    for (auto& v : cell->values)
    {
        s7_gc_unprotect(interpreter, v.second.value);
    }
}


bool Interpreter::eval(const CellKey& key, Dependencies* deps)
{
    deps->clear(key);

    auto env = key.first;
    auto cell = key.second;

    // Input cells are evaluated in their parent's environment and with an
    // input string from their containing Instance
    std::string expr;
    s7_pointer value = nullptr;
    if (isInput(cell))
    {
        env.pop_back();
        if (!env.size())
        {
            value = s7_eval_c_string(interpreter,
                    "(list 'error \"Input at top level\")");
        }
        else
        {
            // If the parent instance doesn't have an expression for this cell,
            // use the default expression, e.g. "0" for (input 0)
            if (!key.first.back()->inputs.count(cell))
            {
                key.first.back()->inputs[cell] = defaultExpr(cell);
            }
            expr = key.first.back()->inputs[cell];
        }
    }
    else
    {
        expr = cell->expr;
    }

    if (value == nullptr)
    {
        auto bindings = s7_nil(interpreter);
        for (auto& c : env.back()->sheet->cells.left)
        {
            // Get a value thunk for this cell
            auto thunk = valueThunk({env, c.second}, key, deps);

            // Then push it to the list
            bindings = s7_cons(interpreter, thunk, bindings);

            // Prepend the symbol name to the bindings list
            bindings = s7_cons(interpreter,
                    s7_make_symbol(interpreter, c.first.c_str()),
                    bindings);
        }

        // Then, install instances into the inlet
        for (auto& i : env.back()->sheet->instances.left)
        {
            s7_pointer values = s7_nil(interpreter);

            {   // Create a temporary environment inside the instance
                auto env_ = env;
                env_.push_back(i.second);
                for (const auto& c : i.second->sheet->cells.left)
                {
                    if (c.second->type == Cell::OUTPUT)
                    {
                        auto pair = s7_cons(interpreter,
                                s7_make_symbol(interpreter, c.first.c_str()),
                                valueThunk({env_, c.second}, key, deps));
                        values = s7_cons(interpreter, pair, values);
                    }
                }
            }

            // If this interpreter has outputs, then build a thunk
            auto args = s7_list(interpreter, 5,
                s7_make_c_pointer(interpreter, deps),
                encode_name_key(interpreter, {env, i.first}),
                encode_cell_key(interpreter, key),
                check_upstream,
                values);
            s7_pointer thunk = s7_call(interpreter, instance_thunk_factory, args);

            // Then push it to the list
            bindings = s7_cons(interpreter, thunk, bindings);

            // Prepend the symbol name to the bindings list
            bindings = s7_cons(interpreter,
                    s7_make_symbol(interpreter, i.first.c_str()),
                    bindings);
        }

        // Run the evaluation and get out a value
        auto args = s7_list(interpreter, 2,
                s7_make_string(interpreter, expr.c_str()),
                s7_inlet(interpreter, bindings));
        value = s7_call(interpreter, eval_func, args);
    }

    // If the output is not tagged as a value, handle the error, return it
    const bool valid = s7_is_eqv(s7_car(value),
                                 s7_make_symbol(interpreter, "value"));
    if (!valid)
    {
        // If the error was due to an unbound variable, then record the
        // attempted lookup in our dependencies table.
        if (s7_is_eqv(s7_car(value),
                      s7_make_symbol(interpreter, "unbound-var")))
        {
            auto target = s7_object_to_c_string(interpreter, s7_caddr(value));
            deps->insert(key, {env, std::string(target)});
            free(target);
        }
        else if (s7_is_eqv(s7_car(value),
                      s7_make_symbol(interpreter, "unbound-instance")))
        {
            auto target = s7_object_to_c_string(interpreter, s7_caddr(value));
            auto instance = s7_string(s7_cadddr(value));

            // If we failed to get a variable from an instance, then we need
            // to push a nested dependency into that instance
            auto env_ = env;
            env_.push_back(env.back()->sheet->instances.left.at(instance));
            deps->insert(key, {env_, std::string(target)});
            free(target);
        }
    }

    // If the value is the same, return false
    if (cell->values.count(key.first) &&
        s7_is_equal(interpreter, value, cell->values[key.first].value))
    {
        return false;
    }
    // Otherwise, swap out the value and return true
    else
    {
        if (cell->values.count(key.first))
        {
            s7_gc_unprotect(interpreter, cell->values[key.first].value);
        }
        cell->values[key.first].value = value;
        cell->values[key.first].valid = valid;
        s7_gc_protect(interpreter, cell->values[key.first].value);

        // Also get a string representation out
        if (valid)
        {
            char* str_ptr = s7_object_to_c_string(interpreter, s7_cdr(value));
            cell->values[key.first].str = std::string(str_ptr);
            free(str_ptr);
        }
        else
        {
            cell->values[key.first].str = std::string(
                    s7_format(interpreter, s7_cdr(value)));
        }

        return true;
    }
}

bool Interpreter::isInput(const Cell* cell) const
{
    return
        s7_is_eq(s7_t(interpreter),
            s7_call(interpreter, is_input,
                s7_list(interpreter, 1,
                    s7_make_string(interpreter, cell->expr.c_str()))));
}

bool Interpreter::isOutput(const Cell* cell) const
{
    return
        s7_is_eq(s7_t(interpreter),
            s7_call(interpreter, is_output,
                s7_list(interpreter, 1,
                    s7_make_string(interpreter, cell->expr.c_str()))));
}

s7_pointer Interpreter::valueThunk(const CellKey& lookee, const CellKey& looker,
                                   Dependencies* deps)
{
    const auto& c = lookee.second;
    const auto& env = lookee.first;

    // Construct a lookup thunk using value_thunk_factory
    auto args = s7_list(interpreter, 5,
        s7_make_c_pointer(interpreter, deps),
        encode_name_key(interpreter, toNameKey(lookee)),
        encode_cell_key(interpreter, looker),
        check_upstream,
        c->values.count(env) ? c->values.at(env).value
                             : s7_nil(interpreter));
    return s7_call(interpreter, value_thunk_factory, args);
}

std::string Interpreter::defaultExpr(const Cell* cell) const
{
    auto s =
        s7_object_to_c_string(interpreter,
            s7_call(interpreter, default_expr,
                s7_list(interpreter, 1,
                    s7_make_string(interpreter, cell->expr.c_str()))));
    std::string out(s);
    free(s);
    return out;
}

Cell::Type Interpreter::cellType(const Cell* cell) const
{
    return isInput(cell)  ? Cell::INPUT :
           isOutput(cell) ? Cell::OUTPUT :
                            Cell::BASIC;
}

bool Interpreter::nameValid(const std::string& str) const
{
    return
        s7_is_eq(s7_t(interpreter),
            s7_call(interpreter, name_valid,
                s7_list(interpreter, 1,
                    s7_make_string(interpreter, str.c_str()))));
}


}   //  namespace Graph
