// See the file "COPYING" in the main distribution directory for copyright.

#pragma once

#include "Stmt.h"
#include "Val.h"


class NameExpr;
class ConstExpr;

// Class representing a single compiled statement.  Designed to
// be fully opaque, but also effective without requiring pointer
// management.
class CompiledStmt {
protected:
	friend class AbstractMachine;

	CompiledStmt(int _stmt_num)	{ stmt_num = _stmt_num; }

	int stmt_num;	// used for AbstractMachine
};


class OpaqueVals;

class StmtCompiler : public Stmt {
public:
	virtual const CompiledStmt Print(OpaqueVals* v) = 0;

	virtual const CompiledStmt ReturnV(const NameExpr* n) = 0;
	virtual const CompiledStmt ReturnC(const ConstExpr* c) = 0;
	virtual const CompiledStmt ReturnX() = 0;

	// Returns a handle to state associated with building
	// up a list of values.
	virtual OpaqueVals* BuildVals(const IntrusivePtr<ListExpr>&) = 0;

	TraversalCode Traverse(TraversalCallback* cb) const override;
};


class AbstractStmt;
union AS_ValUnion;

class AbstractMachine : public StmtCompiler {
public:
	AbstractMachine(int frame_size);
	~AbstractMachine() override;

	const CompiledStmt Print(OpaqueVals* v) override;

	const CompiledStmt ReturnV(const NameExpr* n) override;
	const CompiledStmt ReturnC(const ConstExpr* c) override;
	const CompiledStmt ReturnX() override;

	OpaqueVals* BuildVals(const IntrusivePtr<ListExpr>&) override;

	void StmtDescribe(ODesc* d) const override;

	IntrusivePtr<Val> Exec(Frame* f, stmt_flow_type& flow) const override;

protected:
	// Returns a new value constructed from the given value and
	// accompanying type.  The value has the proper subclass if
	// need be.
	IntrusivePtr<Val> ASValToVal(const AS_ValUnion& u,
					const BroType* t) const;

	union AS_ValUnion ValToASVal(Val* v, const BroType* t) const;

	void SyncGlobals();

	const CompiledStmt AddStmt(const AbstractStmt& stmt);

	int FrameSlot(const ID* id);
	int RegisterSlot();

	vector<AbstractStmt> stmts;
	AS_ValUnion* frame;
	int frame_size;
};
