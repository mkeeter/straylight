#include <cstdlib>

#include "s7/s7.h"

#include "graph/interpreter.hpp"

Interpreter::Interpreter()
    : interpreter(s7_init()),
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
