#include <cstdlib>

#include "s7/s7.h"

#include "graph/interpreter.hpp"
#include "graph/dependencies.hpp"
#include "graph/root.hpp"

namespace Graph {

// Forward declarations
static bool set_insert_(const char* symbol_name, void* data);

////////////////////////////////////////////////////////////////////////////////

struct ValueThunk {
    NameKey target;
    ValuePtr value;

    CellKey looker;
    Dependencies* deps;
    static int tag;

    static void free(void* v)
        { delete static_cast<ValueThunk*>(v); }

    static char* print(s7_scheme* sc, void* s)
    {
        (void)sc;
        return Interpreter::print("value-thunk", s);
    }

    static s7_pointer apply(s7_scheme* sc, s7_pointer obj, s7_pointer args)
    {
        (void)args;

        auto out = static_cast<ValueThunk*>(s7_object_value_checked(obj, tag));
        assert(out);
        return apply(sc, out);
    }

    static s7_pointer apply(s7_scheme* sc, ValueThunk* out)
    {
        if (out->deps->insert(out->looker, out->target))
        {
            return s7_error(sc, s7_make_symbol(sc, "circular-lookup"),
                    s7_list(sc, 1, s7_make_string(sc, "Circular lookup")));
        }

        return (s7_is_eqv(s7_car(out->value), s7_make_symbol(sc, "value")))
            ? s7_cdr(out->value)
            : s7_error(sc, s7_make_symbol(sc, "invalid-lookup"),
                    s7_list(sc, 1, s7_make_string(sc, "Invalid lookup")));
    }
};
int ValueThunk::tag = -1;

struct InstanceThunk {
    NameKey target;
    InstanceIndex index;
    std::map<std::string, ValueThunk> values;

    CellKey looker;
    Dependencies* deps;
    static int tag;

    static void free(void* v)
        { delete static_cast<InstanceThunk*>(v); }

    static char* print(s7_scheme* sc, void* v)
    {
        (void)sc;
        return Interpreter::print("instance-thunk", v);
    }

    static s7_pointer apply(s7_scheme* sc, s7_pointer obj, s7_pointer args)
    {
        if (s7_list_length(sc, args) != 1)
        {
            return s7_wrong_number_of_args_error(sc,
                    "instance-thunk apply: wrong number of args: (~A)", args);
        }
        else if (!s7_is_symbol(s7_car(args)))
        {
            return s7_wrong_type_arg_error(sc, "instance-thunk apply", 0,
                    s7_car(args), "symbol");
        }

        auto out = static_cast<InstanceThunk*>(
                s7_object_value_checked(obj, tag));
        assert(out);

        // Record a lookup of the instance, to detect instance name changes
        if (out->deps->insert(out->looker, out->target))
        {
            return s7_error(sc, s7_make_symbol(sc, "circular-lookup"),
                    s7_list(sc, 1, s7_make_string(sc, "Circular lookup")));
        }

        auto key = s7_symbol_name(s7_car(args));
        if (out->values.count(key) == 0)
        {
            // Record the failed lookup
            auto env_ = out->target.first;
            env_.push_back(out->index);

            out->deps->insert(out->looker, {env_, key});
            return s7_error(sc, s7_make_symbol(sc, "missing-lookup"),
                    s7_list(sc, 1, s7_make_string(sc, "Missing instance lookup")));
        }

        return ValueThunk::apply(sc, &out->values.at(key));
    }
};
int InstanceThunk::tag = -1;

struct SheetResultThunk
{
    std::map<std::string, Value> values;

    static void free(void* v)
        { delete static_cast<SheetResultThunk*>(v); }

    static char* print(s7_scheme* sc, void* v)
    {
        (void)sc;
        return Interpreter::print("sheet-result-thunk", v);
    }

    static s7_pointer apply(s7_scheme* sc, s7_pointer obj, s7_pointer args)
    {
        // Only one argument is allowed, and it must be a symbol
        if (s7_list_length(sc, args) != 1)
        {
            return s7_wrong_number_of_args_error(sc,
                    "sheet-result-thunk apply: wrong number of args: (~A)", args);
        }
        else if (!s7_is_symbol(s7_car(args)))
        {
            return s7_wrong_type_arg_error(sc, "sheet-result-thunk apply", 0,
                    s7_car(args), "symbol");
        }

        auto out = static_cast<SheetResultThunk*>(
                s7_object_value_checked(obj, tag));
        assert(out);

        // If the key isn't present, then fail immediately
        auto key = s7_symbol_name(s7_car(args));
        if (out->values.count(key) == 0)
        {
            return s7_error(sc, s7_make_symbol(sc, "missing-lookup"),
                    s7_list(sc, 1, s7_make_string(sc, "Missing sheet lookup")));
        }

        // Otherwise, either return the value or throw an error
        // (if the value is invalid)
        auto& v = out->values.at(key);
        return (s7_is_eqv(s7_car(v.value), s7_make_symbol(sc, "value")))
            ? s7_cdr(v.value)
            : s7_error(sc, s7_make_symbol(sc, "invalid-lookup"),
                    s7_list(sc, 1, s7_make_string(sc, "Invalid lookup")));
    }
    static int tag;
};
int SheetResultThunk::tag = -1;

struct SheetThunk
{
    SheetIndex target;

    CellKey looker;
    Root* root;
    static int tag;

    static void free(void* v)
        { delete static_cast<SheetThunk*>(v); }

    static char* print(s7_scheme* sc, void* v)
    {
        (void)sc;
        return Interpreter::print("sheet-thunk", v);
    }

    static s7_pointer apply(s7_scheme* sc, s7_pointer obj, s7_pointer args)
    {
        auto out = static_cast<SheetThunk*>(s7_object_value_checked(obj, tag));
        assert(out);

        std::string err;
        std::list<Value> args_;
        auto sym = s7_make_symbol(sc, "value");
        for (auto a = args; a != s7_nil(sc); a = s7_cdr(a))
        {
            args_.push_back(Value(s7_cons(sc, sym, s7_car(a)), "", true));
        }
        auto vals = out->root->callSheet(out->looker, out->target, args_, &err);
        if (err.length() == 0)
        {
            return s7_make_object(sc, SheetResultThunk::tag,
                    new SheetResultThunk { vals });
        }
        else
        {
            return s7_error(sc, s7_make_symbol(sc, "call-sheet"),
                    s7_list(sc, 1, s7_make_string(sc, err.c_str())));
        }
    }
};
int SheetThunk::tag = -1;

////////////////////////////////////////////////////////////////////////////////

Interpreter::Interpreter(Root& parent, Dependencies* deps)
    : root(parent), deps(deps), sc(s7_init()),
      is_input(s7_eval_c_string(sc, R"(
        (lambda (str)
          (catch #t
            (lambda ()
              (let ((r (read (open-input-string str))))
                (eqv? (car r) 'input)))
            (lambda args #f)))
      )")),
      is_output(s7_eval_c_string(sc, R"(
        (lambda (str)
          (catch #t
            (lambda ()
              (let ((r (read (open-input-string str))))
                (eqv? (car r) 'output)))
            (lambda args #f)))
      )")),
      default_expr(s7_eval_c_string(sc, R"(
        (lambda (str)
          (catch #t
            (lambda ()
              (let ((r (read (open-input-string str))))
                (if (eqv? (car r) 'input)
                    (cadr r)
                    "")))
            (lambda args "")))
      )")),
      name_valid(s7_eval_c_string(sc, R"(
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
      eval_func(s7_eval_c_string(sc, R"(
        (lambda (str eval-env cell-env)
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
                     (r (cons 'begin (*cell-reader* j cell-env))))
                (cons 'value (eval r eval-env))))
            (lambda args
              (cond ((string=? "~A: unbound variable" (caadr args))
                        (copy (cons 'unbound-var (cadr args))))
                    (else
                        (copy (cons 'error (cadr args))))))))
      )"))
{
    // Install default *cell-reader*
    setReader(s7_eval_c_string(sc, "(lambda (s c) s)"));

    for (s7_pointer ptr: {is_input, is_output, default_expr, name_valid,
                          eval_func})
    {
        s7_gc_protect(sc, ptr);
    }

    ValueThunk::tag = s7_new_type_x(sc, "value-thunk",
        ValueThunk::print,
        ValueThunk::free,
        nullptr,  /* equal */
        nullptr,  /* gc_mark */
        ValueThunk::apply,  /* apply */
        nullptr,  /* set */
        nullptr,  /* length */
        nullptr,  /* copy */
        nullptr,  /* reverse */
        nullptr); /* fill */

    InstanceThunk::tag = s7_new_type_x(sc, "instance-thunk",
        InstanceThunk::print,
        InstanceThunk::free,
        nullptr,  /* equal */
        nullptr,  /* gc_mark */
        InstanceThunk::apply,  /* apply */
        nullptr,  /* set */
        nullptr,  /* length */
        nullptr,  /* copy */
        nullptr,  /* reverse */
        nullptr); /* fill */

    SheetThunk::tag = s7_new_type_x(sc, "sheet-thunk",
        SheetThunk::print,
        SheetThunk::free,
        nullptr,  /* equal */
        nullptr,  /* gc_mark */
        SheetThunk::apply,  /* apply */
        nullptr,  /* set */
        nullptr,  /* length */
        nullptr,  /* copy */
        nullptr,  /* reverse */
        nullptr); /* fill */

    SheetResultThunk::tag = s7_new_type_x(sc, "sheet-result-thunk",
        SheetResultThunk::print,
        SheetResultThunk::free,
        nullptr,  /* equal */
        nullptr,  /* gc_mark */
        SheetResultThunk::apply,  /* apply */
        nullptr,  /* set */
        nullptr,  /* length */
        nullptr,  /* copy */
        nullptr,  /* reverse */
        nullptr); /* fill */
}

Interpreter::~Interpreter()
{
    free(sc);
}

bool Interpreter::isInput(const std::string& expr) const
{
    return
        s7_is_eq(s7_t(sc),
            s7_call(sc, is_input,
                s7_list(sc, 1,
                    s7_make_string(sc, expr.c_str()))));
}

bool Interpreter::isOutput(const std::string& expr) const
{
    return
        s7_is_eq(s7_t(sc),
            s7_call(sc, is_output,
                s7_list(sc, 1,
                    s7_make_string(sc, expr.c_str()))));
}

std::string Interpreter::defaultExpr(const std::string& expr) const
{
    auto s =
        s7_object_to_c_string(sc,
            s7_call(sc, default_expr,
                s7_list(sc, 1,
                    s7_make_string(sc, expr.c_str()))));
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
        s7_is_eq(s7_t(sc),
            s7_call(sc, name_valid,
                s7_list(sc, 1,
                    s7_make_string(sc, str.c_str()))));
}

Value Interpreter::eval(const CellKey& key)
{
    deps->clear(key);

    auto env = key.first;
    auto cell = key.second;

    // Input cells are evaluated in their parent's environment and with an
    // input string from their containing Instance
    std::string expr = root.getTree().at(cell).cell()->expr;
    s7_pointer value = nullptr;
    if (isInput(expr))
    {
        if (env.size() == 1)
        {
            value = s7_eval_c_string(sc,
                    "(list 'error \"Input at top level\")");
        }
        else
        {
            // Get the input expression for the given cell key
            // (which lives in the parent instance)
            const auto& parent = root.getTree().at(env.back());
            assert(parent.instance());
            expr = parent.instance()->inputs.at(cell);
            env.pop_back();
        }
    }

    // If we didn't get an error, then make bindings for all symbols
    if (value == nullptr)
    {
        auto bindings = s7_nil(sc); // empty list
        const auto& sheet = root.getTree().at(env.back()).instance()->sheet;
        for (auto& i : root.getTree().iterItems(sheet))
        {
            // Prepend (symbol name, thunk) to the bindings list
            bindings = s7_cons(sc,
                    s7_make_symbol(sc, root.getTree().nameOf(i).c_str()),
                    s7_cons(sc, getItemThunk(env, i, key),
                    bindings));
        }

        for (auto& i : root.getTree().sheetsAbove(env))
        {
            bindings = s7_cons(sc,
                    s7_make_symbol(sc, root.getTree().nameOf(i).c_str()),
                    s7_cons(sc, getSheetThunk(i, key),
                    bindings));
        }

        // Run the evaluation and get out a value
        auto args = s7_list(sc, 3,
                s7_make_string(sc, expr.c_str()),
                s7_inlet(sc, bindings),
                s7_make_c_pointer(sc, (void*)&key));
        value = s7_call(sc, eval_func, args);
    }

    // If the output is not tagged as a value, handle the error
    const bool valid = s7_is_eqv(s7_car(value),
                                 s7_make_symbol(sc, "value"));
    if (!valid)
    {
        // If the error was due to an unbound variable, then record the
        // attempted lookup in our dependencies table.
        if (s7_is_eqv(s7_car(value),
                      s7_make_symbol(sc, "unbound-var")))
        {
            auto target_ = s7_object_to_c_string(sc, s7_caddr(value));
            std::string target(target_);
            free(target_);

            if (root.isItemName(target))
            {
                deps->insert(key, {env, std::string(target)});
            }
            else if (root.isSheetName(target))
            {
                // The target sheet could be inserted in any of the parent
                // environments, so we'll insert a dependency on all of them
                Env env_;
                for (const auto& e : env)
                {
                    env_.push_back(e);
                    deps->insert(key, {env_, std::string(target)});
                }
            }
        }
    }

    // If the value is present and unchanged, return false
    auto c = root.getTree().at(cell).cell();
    if (c->values.count(env) &&
        s7_is_equal(sc, value, c->values.at(env).value))
    {
        return Value(nullptr, "", false);
    }
    // Otherwise, swap out the value and return true
    else
    {
        // Protect the new value from the GC
        s7_gc_protect(sc, value);

        // Get a string representation out
        std::string repr;
        if (valid)
        {
            char* str_ptr = s7_object_to_c_string(sc, s7_cdr(value));
            repr = std::string(str_ptr);
            free(str_ptr);
        }
        else
        {
            repr = s7_format(sc, s7_cdr(value));
        }
        return {value, repr, valid};
    }
}

void Interpreter::release(ValuePtr v)
{
    s7_gc_unprotect(sc, v);
}

ValuePtr Interpreter::untag(ValuePtr v)
{
    return s7_cdr(v);
}

void Interpreter::setReader(s7_cell* r)
{
    s7_gc_unprotect(sc, s7_name_to_value(sc, "*cell-reader*"));
    s7_gc_protect(sc, r);
    s7_define_variable(sc, "*cell-reader*", r);
}

s7_cell* Interpreter::getItemThunk(const Env& env, const ItemIndex& index,
                                   const CellKey& looker)
{
    const auto& item = root.getTree().at(index);
    if (auto cell = item.cell())
    {
        // Construct a lookup thunk and return it immediately
        return s7_make_object(sc, ValueThunk::tag, new ValueThunk {
            {env, root.getTree().nameOf(index)},
            cell->values.count(env) ? cell->values.at(env).value
                                    : s7_nil(sc),
            looker, deps,
        });
    }
    else if (auto instance = item.instance())
    {
        std::map<std::string, ValueThunk> values;

        {   // Create a temporary environment inside the instance, and
            // build up a list of OUTPUT cells within this instance
            auto env_ = env;
            env_.push_back(InstanceIndex(index));
            for (const auto& c : root.getTree().iterItems(instance->sheet))
            {
                if (auto cell = root.getTree().at(c).cell())
                {
                    if (cell->type >= Cell::INPUT)
                    {
                        values[root.getTree().nameOf(c)] = ValueThunk {
                            root.toNameKey({env_, CellIndex(c)}),
                            cell->values.count(env_)
                                ? cell->values.at(env_).value
                                : s7_nil(sc),
                            looker, deps,
                        };
                    }
                }
            }
        }

        return s7_make_object(sc, InstanceThunk::tag,
            new InstanceThunk {
                {env, root.getTree().nameOf(index)},
                InstanceIndex(index),
                values,
                looker, deps,
        });
    }

    assert(false); // Item index must be either a cell or an instance
    return s7_nil(sc);
}

s7_cell* Interpreter::getSheetThunk(const SheetIndex& index,
                                    const CellKey& looker)
{
    return s7_make_object(sc, SheetThunk::tag, new SheetThunk {
        index, looker, &root
    });
}

////////////////////////////////////////////////////////////////////////////////

bool Interpreter::isReserved(const std::string& k) const
{
    // For simplicity, reserve all names starting in #
    return (k.size() && k[0] == '#') || k == "input" || k == "output" ||
           s7_name_to_value(sc, k.c_str()) != s7_undefined(sc);
}

std::set<std::string> Interpreter::keywords() const
{
    // Hard-code special keywords, then look up the rest
    std::set<std::string> keywords = {
        "#t", "#f", "#<unspecified>", "#<undefined>",
        "#<eof>", "#true", "#false"};
    std::pair<s7_scheme*, decltype(keywords)*> data = {sc, &keywords};
    s7_for_each_symbol_name(sc, set_insert_, &data);
    return keywords;
}

char* Interpreter::print(const std::string& type, void* s)
{
    std::stringstream ss;
    ss << "#<" << type << " at " << s << ">";
    auto str = ss.str();

    auto out = static_cast<char*>(calloc(str.length() + 1, sizeof(char)));
    memcpy(out, &str[0], str.length());
    return out;
}

////////////////////////////////////////////////////////////////////////////////

static bool set_insert_(const char* symbol_name, void* data)
{
    auto& d = *reinterpret_cast<std::pair<s7_scheme*,
                                std::set<std::string>*>*>(data);
    if (s7_name_to_value(d.first, symbol_name) != s7_undefined(d.first))
    {
        d.second->insert(symbol_name);
    }
    return false;
}

}   // namespace Graph
