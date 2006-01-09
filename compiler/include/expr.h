#ifndef _EXPR_H_
#define _EXPR_H_

#include <stdio.h>
#include "alist.h"
#include "analysis.h"
#include "baseAST.h"
#include "symbol.h"
#include "primitive.h"

class Stmt;
class AAST;
class FnSymbol;

enum PartialTag {
  PARTIAL_OK,
  PARTIAL_NEVER,
  PARTIAL_ALWAYS
};

enum OpTag {
  OP_NONE,
  OP_UNPLUS,
  OP_UNMINUS,
  OP_LOGNOT,
  OP_BITNOT,
  OP_PLUS,
  OP_MINUS,
  OP_MULT,
  OP_DIV,
  OP_MOD,
  OP_EQUAL,
  OP_NEQUAL,
  OP_LEQUAL,
  OP_GEQUAL,
  OP_LTHAN,
  OP_GTHAN,
  OP_BITAND,
  OP_BITOR,
  OP_BITXOR,
  OP_LOGAND,
  OP_LOGOR,
  OP_EXP,
  OP_SEQCAT,
  OP_BY,
  OP_SUBTYPE,
  OP_NOTSUBTYPE,
  OP_GET_MEMBER,
  OP_SET_MEMBER,
  OP_INIT,
  OP_MOVE
};

extern char* opChplString[];

#define OP_ISBINARYOP(op) (op >= OP_PLUS && op <= OP_NOTSUBTYPE)
#define OP_ISUNARYOP(op) (op >= OP_UNPLUS && op <= OP_BITNOT)

#define OP_ISLOGICAL(op) \
  ((op == OP_LOGNOT) ||  \
   (op == OP_EQUAL)  ||  \
   (op == OP_NEQUAL) ||  \
   (op == OP_LEQUAL) ||  \
   (op == OP_GEQUAL) ||  \
   (op == OP_LTHAN)  ||  \
   (op == OP_GTHAN)  ||  \
   (op == OP_LOGAND) ||  \
   (op == OP_LOGOR))

class Expr : public BaseAST {
 public:
  Stmt* parentStmt;
  Expr* parentExpr;
  AAST *ainfo;

  Expr(astType_t astType = EXPR);
  COPY_DEF(Expr);
  virtual void callReplaceChild(BaseAST* new_ast);
  virtual void verify(void); 
  virtual void traverse(Traversal* traversal, bool atTop = true);
  virtual void traverseDef(Traversal* traversal, bool atTop = true);
  virtual void traverseExpr(Traversal* traversal);
  virtual ASTContext getContext(void);
  virtual Type* typeInfo(void);

  virtual bool isParam(void);
  virtual bool isConst(void);
  virtual int rank(void);

  bool isRef(void);
  Stmt* Expr::getStmt();

  void codegenCastToString(FILE* outfile);
};
#define forv_Expr(_p, _v) forv_Vec(Expr, _p, _v)

class DefExpr : public Expr {
 public:
  Symbol* sym;
  Expr* init;
  Expr* exprType;
  Vec<FnSymbol *> initAssign;

  DefExpr(Symbol* initSym = NULL,
          Expr* initInit = NULL,
          Expr* initExprType = NULL);
  virtual void verify(void); 
  COPY_DEF(DefExpr);
  virtual void replaceChild(BaseAST* old_ast, BaseAST* new_ast);
  void traverseExpr(Traversal* traversal);

  Type* typeInfo(void);

  void print(FILE* outfile);
  bool noCodegen() { return true; }
  void codegen(FILE* outfile);
};


class SymExpr : public Expr {
 public:
  Symbol* var;
  SymExpr(Symbol* init_var);
  SymExpr(char* init_var);
  COPY_DEF(SymExpr);
  virtual void replaceChild(BaseAST* old_ast, BaseAST* new_ast);
  virtual void verify(void); 
  void traverseExpr(Traversal* traversal);

  Type* typeInfo(void);
  virtual bool isConst(void);
  virtual bool isParam(void);
  void print(FILE* outfile);
  void codegen(FILE* outfile);
};


class CallExpr : public Expr {
 public:
  Expr* baseExpr;
  AList<Expr>* argList;
  OpTag opTag;
  PrimitiveOp *primitive;
  PartialTag partialTag;
  Symbol* member;
  Type* member_type;
  int member_offset;

  CallExpr(BaseAST* base, BaseAST* arg1 = NULL, BaseAST* arg2 = NULL,
           BaseAST* arg3 = NULL, BaseAST* arg4 = NULL);
  CallExpr(OpTag initOpTag, BaseAST* arg1, BaseAST* arg2 = NULL, BaseAST *arg3 = NULL);
  CallExpr(PrimitiveOp *prim, BaseAST* arg1 = NULL, BaseAST* arg2 = NULL);
  CallExpr(char* name, BaseAST* arg1 = NULL, BaseAST* arg2 = NULL,
           BaseAST* arg3 = NULL, BaseAST* arg4 = NULL);
  virtual void verify(void); 
  COPY_DEF(CallExpr);

  virtual void replaceChild(BaseAST* old_ast, BaseAST* new_ast);
  void traverseExpr(Traversal* traversal);

  virtual void print(FILE* outfile);
  virtual void codegen(FILE* outfile);

  void makeOp(void);
  bool isAssign(void);
  bool isOp(OpTag);

  FnSymbol* isResolved(void);
  bool isNamed(char*);

  Expr* get(int index);
  FnSymbol* findFnSymbol(void);
  Type* typeInfo(void);
  bool isPrimitive(void);
};


class CastExpr : public Expr {
 public:
  Expr* expr;
  Expr* newType;
  Type* type;

  CastExpr(Expr* initExpr, Expr* initNewType, Type* initType = dtUnknown);
  virtual void verify(void); 
  COPY_DEF(CastExpr);
  virtual void replaceChild(BaseAST* old_ast, BaseAST* new_ast);
  void traverseExpr(Traversal* traversal);

  Type* typeInfo(void);

  void print(FILE* outfile);
  void codegen(FILE* outfile);
};


class CondExpr : public Expr {
 public:
  Expr* condExpr;
  Expr* thenExpr;
  Expr* elseExpr;

  CondExpr(Expr* initCondExpr, Expr* initThenExpr, Expr* initElseExpr = NULL);
  virtual void verify(void); 
  COPY_DEF(CondExpr);
  virtual void replaceChild(BaseAST* old_ast, BaseAST* new_ast);
  void traverseExpr(Traversal* traversal);
  Type* typeInfo(void);
  void print(FILE* outfile);
  void codegen(FILE* outfile);
};


class NamedExpr : public Expr {
 public:
  char* name;
  Expr* actual;
  NamedExpr(char* init_name, Expr* init_actual);
  virtual void verify(void); 
  COPY_DEF(NamedExpr);
  virtual void replaceChild(BaseAST* old_ast, BaseAST* new_ast);
  void traverseExpr(Traversal* traversal);
  Type* typeInfo(void);
  void print(FILE* outfile);
  void codegen(FILE* outfile);
};


enum ImportTag {
  IMPORT_WITH,
  IMPORT_USE
};


class ImportExpr : public Expr {
 public:
  ImportTag importTag;          // true -> with, false -> use
  Expr* expr;                   // Module or class withed or used
  char* version;                // Module version
  bool only;                    // only on renameList
  Map<char*,char*>* renameList; // only clause
  Vec<char*>* exceptList;       // except clause
  ImportExpr(ImportTag initImportTag, Expr* initExpr);
  virtual void verify(void); 
  COPY_DEF(ImportExpr);
  virtual void replaceChild(BaseAST* old_ast, BaseAST* new_ast);
  void traverseExpr(Traversal* traversal);
  Type* typeInfo(void);
  void print(FILE* outfile);
  void codegen(FILE* outfile);
  ModuleSymbol* getImportedModule(void);
  ClassType* getStruct(void);
};


void initExpr(void);
Expr *new_BoolLiteral(bool b);
Expr *new_IntLiteral(char *);
Expr *new_IntLiteral(int i);
Expr *new_FloatLiteral(char *n, double d);
Expr *new_StringLiteral(char *str);
bool get_int(Expr *e, long *i); // false is failure
bool get_string(Expr *e, char **s); // false is failure
VarSymbol *get_constant(Expr *e);
AList<DefExpr>* exprsToIndices(AList<Expr>* indices);

#endif
