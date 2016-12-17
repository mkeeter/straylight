#include <cstdlib>

#include "s7/s7.h"

#include "graph/interpreter.hpp"
#include "graph/dependencies.hpp"
#include "graph/root.hpp"

// Forward declarations
static s7_pointer check_upstream_(s7_scheme* interpreter, s7_pointer args);

////////////////////////////////////////////////////////////////////////////////

Interpreter::Interpreter(const Root& parent, Dependencies* deps)
    : root(parent), deps(deps), interpreter(s7_init()),
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
      )")),
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
                         ((eqv? 'value (car value)) (cdr value))
                         (else (error 'invalid-lookup "Invalid lookup"))))))
          )")),
      instance_thunk_factory(s7_eval_c_string(interpreter, R"(
          (lambda (deps target target-index looker check-upstream values)
            (let ((lookup (apply hash-table values))
                  (keys (apply hash-table
                    (map (lambda (v) (cons (car v) #t)) values))))
            (lambda (key)
              (let ((res (check-upstream deps target looker)))
                (cond  ((=  1 res) (error 'circular-lookup "Circular lookup"))
                       ((not (hash-table-ref keys key))
                         (error 'missing-lookup "~A: missing instance lookup in ~A (~A) " key (car target) target-index))
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
                    ((string=? "~A: missing instance lookup in ~A (~A) " (caadr args))
                        (copy (cons 'unbound-instance (cadr args))))
                    (else
                        (copy (cons 'error (cadr args))))))))
      )"))
{
    for (s7_pointer ptr: {is_input, is_output, default_expr})
    {
        s7_gc_protect(interpreter, ptr);
    }
}

Interpreter::~Interpreter()
{
    free(interpreter);
}

bool Interpreter::isInput(const std::string& expr) const
{
    return
        s7_is_eq(s7_t(interpreter),
            s7_call(interpreter, is_input,
                s7_list(interpreter, 1,
                    s7_make_string(interpreter, expr.c_str()))));
}

bool Interpreter::isOutput(const std::string& expr) const
{
    return
        s7_is_eq(s7_t(interpreter),
            s7_call(interpreter, is_output,
                s7_list(interpreter, 1,
                    s7_make_string(interpreter, expr.c_str()))));
}

std::string Interpreter::defaultExpr(const std::string& expr) const
{
    auto s =
        s7_object_to_c_string(interpreter,
            s7_call(interpreter, default_expr,
                s7_list(interpreter, 1,
                    s7_make_string(interpreter, expr.c_str()))));
    std::string out(s);
    free(s);
    return out;
}

Cell::Type Interpreter::cellType(const std::string& expr) const
{
    return isInput(expr)  ? Cell::INPUT :
           isOutput(expr) ? Cell::OUTPUT :
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

Value Interpreter::eval(const CellKey& key)
{
    deps->clear(key);

    auto env = key.first;
    auto cell = key.second;

    // Input cells are evaluated in their parent's environment and with an
    // input string from their containing Instance
    std::string expr = root.getItem(cell).cell()->expr;
    s7_pointer value = nullptr;
    if (isInput(expr))
    {
        if (env.size() == 1)
        {
            value = s7_eval_c_string(interpreter,
                    "(list 'error \"Input at top level\")");
        }
        else
        {
            // Get the input expression for the given cell key
            // (which lives in the parent instance)
            const auto& parent = root.getItem(env.back());
            expr = parent.instance()->inputs.at(cell);
            env.pop_back();
        }
    }

    if (value == nullptr)
    {
        auto bindings = s7_nil(interpreter); // empty list
        const auto& sheet = root.getItem(env.back()).instance()->sheet;
        for (auto& i : root.iterItems(sheet))
        {
            // Prepend (symbol name, thunk) to the bindings list
            bindings = s7_cons(interpreter,
                    s7_make_symbol(interpreter, root.nameOf(i).c_str()),
                    s7_cons(interpreter, getThunk(env, i, key),
                    bindings));
        }

        // Run the evaluation and get out a value
        auto args = s7_list(interpreter, 2,
                s7_make_string(interpreter, expr.c_str()),
                s7_inlet(interpreter, bindings));
        value = s7_call(interpreter, eval_func, args);
    }

    // If the output is not tagged as a value, handle the error
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
            // If we failed to get a variable from an instance, then we need
            // to push a nested dependency into that instance
            auto args = s7_cddr(value);
            auto target = s7_object_to_c_string(interpreter, s7_car(args));
            auto instance_index = s7_integer(s7_caddr(args));

            // Look up the instance index by name and push it into the env
            auto env_ = env;
            env_.push_back({(unsigned)instance_index});

            deps->insert(key, {env_, std::string(target)});
            free(target);
        }
    }

    // If the value is present and unchanged, return false
    auto c = root.getItem(cell).cell();
    if (c->values.count(env) &&
        s7_is_equal(interpreter, value, c->values.at(env).value))
    {
        return Value(nullptr, "", false);
    }
    // Otherwise, swap out the value and return true
    else
    {
        // Protect the new value from the GC
        s7_gc_protect(interpreter, value);

        // Get a string representation out
        std::string repr;
        if (valid)
        {
            char* str_ptr = s7_object_to_c_string(interpreter, s7_cdr(value));
            repr = std::string(str_ptr);
            free(str_ptr);
        }
        else
        {
            repr = s7_format(interpreter, s7_cdr(value));
        }
        return {value, repr, valid};
    }
}

void Interpreter::release(ValuePtr v)
{
    s7_gc_unprotect(interpreter, v);
}

s7_cell* Interpreter::getThunk(const Env& env, const ItemIndex& index,
                               const CellKey& looker)
{
    const auto& item = root.getItem(index);
    if (auto cell = item.cell())
    {
        CellKey lookee = {env, CellIndex(index.i)};

        // Construct a lookup thunk using value_thunk_factory
        auto args = s7_list(interpreter, 5,
            s7_make_c_pointer(interpreter, deps),
            encodeNameKey(root.toNameKey(lookee)),
            encodeCellKey(looker),
            check_upstream,
            cell->values.count(env) ? cell->values.at(env).value
                                    : s7_nil(interpreter));
        return s7_call(interpreter, value_thunk_factory, args);
    }
    else if (auto instance = item.instance())
    {
        s7_pointer values = s7_nil(interpreter);

        {   // Create a temporary environment inside the instance, and
            // build up a list of OUTPUT cells within this instance
            auto env_ = env;
            env_.push_back(InstanceIndex(index.i));
            for (const auto& c : root.iterItems(instance->sheet))
            {
                if (auto cell = root.getItem(c).cell())
                {
                    if (cell->type == Cell::OUTPUT)
                    {
                        // Prepend (name, value thunk) to the args list
                        auto pair = s7_cons(interpreter,
                                s7_make_symbol(
                                    interpreter, root.nameOf(c).c_str()),
                                getThunk(env_, c, looker));
                        values = s7_cons(interpreter, pair, values);
                    }
                }
            }
        }

        // Build a thunk with instance_thunk_factory
        auto args = s7_list(interpreter, 6,
            s7_make_c_pointer(interpreter, deps),
            encodeNameKey({env, root.nameOf(index)}),
            s7_make_integer(interpreter, index.i),
            encodeCellKey(looker),
            check_upstream,
            values);
        return s7_call(interpreter, instance_thunk_factory, args);
    }

    assert(false); // Item index must be either a cell or an instance
    return s7_nil(interpreter);
}

////////////////////////////////////////////////////////////////////////////////

s7_pointer Interpreter::encodeCellKey(const CellKey& k) const
{
    auto out = s7_nil(interpreter);
    for (auto& i : k.first)
    {
        out = s7_cons(interpreter, s7_make_integer(interpreter, i.i), out);
    }
    out = s7_cons(interpreter, s7_make_integer(interpreter, k.second.i), out);
    return out;
}

s7_pointer Interpreter::encodeNameKey(const NameKey& k) const
{
    auto out = s7_nil(interpreter);
    for (auto& i : k.first)
    {
        out = s7_cons(interpreter, s7_make_integer(interpreter, i.i), out);
    }
    out = s7_cons(interpreter, s7_make_string(interpreter, k.second.c_str()), out);
    return out;
}

CellKey Interpreter::decodeCellKey(s7_scheme* interpreter, s7_pointer v)
{
    CellKey out;
    out.second = s7_integer(s7_car(v));
    v = s7_cdr(v);

    // Because of the list's order, we fill the key from back to front
    out.first.resize(s7_list_length(interpreter, v));
    for (auto itr = out.first.rbegin(); itr != out.first.rend();
         ++itr, v = s7_cdr(v))
    {
        *itr = s7_integer(s7_car(v));
    }

    return out;
}

NameKey Interpreter::decodeNameKey(s7_scheme* interpreter, s7_pointer v)
{
    NameKey out;
    out.second = std::string(s7_string(s7_car(v)));
    v = s7_cdr(v);

    // Because of the list's order, we fill the key from back to front
    out.first.resize(s7_list_length(interpreter, v));
    for (auto itr = out.first.rbegin(); itr != out.first.rend();
         ++itr, v = s7_cdr(v))
    {
        *itr = s7_integer(s7_car(v));
    }

    return out;
}

////////////////////////////////////////////////////////////////////////////////

static s7_pointer check_upstream_(s7_scheme* interpreter, s7_pointer args)
{
    auto& deps = *static_cast<Dependencies*>(s7_c_pointer(s7_car(args)));
    auto lookee = Interpreter::decodeNameKey(interpreter, s7_cadr(args));
    auto looker = Interpreter::decodeCellKey(interpreter, s7_caddr(args));

    return s7_make_integer(interpreter, deps.insert(looker, lookee));
}
