#ifndef _BUILD_H_
#define _BUILD_H_

#include "alist.h"
#include "expr.h"
#include "stmt.h"
#include "symbol.h"

BlockStmt* build_chpl_stmt(AList* stmts);
BlockStmt* build_chpl_stmt(BaseAST* ast = NULL);
ExprStmt* buildLabelStmt(char* name);
ModuleSymbol* build_module(char* name, modType type, AList* stmts);
CallExpr* build_primitive_call(AList* exprs);

FnSymbol* build_if_expr(Expr* e, Expr* e1, Expr* e2 = NULL);
FnSymbol* build_let_expr(BlockStmt* decls, Expr* expr);
BlockStmt* build_while_do_block(Expr* cond, BlockStmt* body);
BlockStmt* build_do_while_block(Expr* cond, BlockStmt* body);
BlockStmt* build_for_block(BlockTag tag,
                           AList* indices,
                           AList* iterators,
                           BlockStmt* body,
                           bool isSquare = false,
                           int only_once = 0);
BlockStmt* build_for_expr(AList* indices,
                          AList* iterators,
                          Expr* expr,
                          bool isSquare = false,
                          Expr* cond = NULL);
BlockStmt* build_param_for_stmt(char* index, Expr* low, Expr* high, Expr* stride, BlockStmt* stmts);
BlockStmt* build_op_assign_chpl_stmt(char* op, Expr* lhs, Expr* rhs);
BlockStmt* build_seqcat_assign_chpl_stmt(Expr* lhs, Expr* rhs);
BlockStmt* build_plus_assign_chpl_stmt(Expr* lhs, Expr* rhs);
BlockStmt* build_minus_assign_chpl_stmt(Expr* lhs, Expr* rhs);
CondStmt* build_select(Expr* s, BlockStmt* whenstmts);
BlockStmt* build_type_select(AList* s, BlockStmt* whenstmts);
FnSymbol* build_reduce(Expr* red, Expr *seq);
FnSymbol* build_scan(Expr* scan, Expr *seq);

void backPropagateInitsTypes(BlockStmt* stmts);
void setVarSymbolAttributes(BlockStmt* stmts,
                            varType vartag,
                            consType constag);

DefExpr* build_class(char* name, Type* type, AList* decls);
DefExpr*
build_arg(intentTag tag, char* ident, Expr* type, Expr* init, Expr* variable);

AList* exprsToIndices(AList* indices);
#endif
