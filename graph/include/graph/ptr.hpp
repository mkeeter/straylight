struct s7_cell;

/*  ValueType is defined here so other classes don't know about s7 */
namespace Graph {
typedef s7_cell* ValuePtr;
};
