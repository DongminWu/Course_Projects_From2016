/* Name Surname */

#include <llvm/Pass.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Instruction.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/InstVisitor.h>
#include <llvm/IR/CFG.h>
#include <llvm/IR/InstIterator.h>
#include <llvm/IR/Constants.h>
#include <llvm/ADT/SmallVector.h>
#include <llvm/ADT/DenseSet.h>
#include <llvm/Support/raw_ostream.h>

using namespace llvm;

enum{
  TYPE_INT = 0,
  TYPE_FLOAT,
  TYPE_DOUBLE,
  TYPE_END
};

enum{
  OP_ADD=8,
  OP_SUB=10,
  OP_FMUL=13,
  OP_END
};

enum
{
  s_OVERDEFINED = 0 ,
  s_UNDEFINED = 1,
  s_CONSTANT = 2,
  s_END = 3
};


namespace
{
#define debug(x) \
  if (false)     \
  {              \
  }              \
  else           \
    errs() << "[debug@"<<__LINE__<<"]" << x << "\n"
#define debug_func_name() \
  if (false)              \
  {                       \
  }                       \
  else                    \
    errs() << "[debug@"<<__LINE__<<"] >>>>" << __FUNCTION__ << "\n"

#define hello___________________world___________________ 1


/* Represents state of a single Value. There are three possibilities:
 *  * undefined: Initial state. Unknown whether constant or not.
 *  * constant: Value is constant.
 *  * overdefined: Value is not constant. */
class State
{
public:
  State() : Kind(UNDEFINED), Const(nullptr) {}

  bool isOverdefined() const { return Kind == OVERDEFINED; }
  bool isUndefined() const { return Kind == UNDEFINED; }
  bool isConstant() const { return Kind == CONSTANT; }
  Constant *getConstant() const
  {
    assert(isConstant());
    return Const;
  }

  void markOverdefined() { Kind = OVERDEFINED; }
  void markUndefined() { Kind = UNDEFINED; }
  void markConstant(Constant *C)
  {
    Kind = CONSTANT;
    Const = C;
  }

  void print(raw_ostream &O) const
  {
    switch (Kind)
    {
    case UNDEFINED:
      O << "undefined";
      break;
    case OVERDEFINED:
      O << "overdefined";
      break;
    case CONSTANT:
      O << "const " << *Const;
      break;
    }
  }

private:
  enum
  {
    OVERDEFINED = 0 ,
    UNDEFINED = 1,
    CONSTANT = 2
  } Kind;
  Constant *Const;
};

raw_ostream &operator<<(raw_ostream &O, const State &S)
{
  S.print(O);
  return O;
}

class ConstPropPass : public FunctionPass, public InstVisitor<ConstPropPass>
{
public:
  static char ID;
  ConstPropPass() : FunctionPass(ID) {}

  virtual void getAnalysisUsage(AnalysisUsage &au) const
  {
    au.setPreservesAll();
  }

  virtual bool runOnFunction(Function &F)
  {
    // TODO Implement constant propagation
    debug_func_name();

    debug("1. set all value to undefined");
    setValueToUndefined(F);
    storeAllInstructionToWorklist(F);
    dumpWorklist();
    dumpStateMap();


    debug("\n\nstart!!!!");
    for (int i =0; i<20;i++)
    {
      debug("===============loop["<<i<<"]start=========");
      if (WorkList.size() == 0) break;
      else{
        Instruction * I = dyn_cast<Instruction>(WorkList.back());
        WorkList.pop_back(); //delete last element
        debug("Instruction@"<<I<<": "<<*I);
        visit(I);
        debug("");
      }

      debug("===============loop["<<i<<"]end===========\n");
    }


    printResults(F);
    return false;
  }

  void visitPHINode(PHINode &Phi)
  {
    // TODO
    debug_func_name();
    debug("getNumOperands = "<<Phi.getNumOperands());
  }

  void visitBinaryOperator(Instruction &I)
  {
    // TODO
    // Hint: ConstExpr::get()
    debug_func_name();
    handleTwoOperandInstruction(I,false);
    // ConstantExpr::get();
  }

  void visitCmpInst(CmpInst &I)
  {
    // TODO
    // Hint: ConstExpr::getCompare()
    debug_func_name();
    debug("getNumOperands = "<<I.getNumOperands());
    debug("getOpcode = "<<I.getOpcode());
    handleTwoOperandInstruction(I,true);
  }

  void visitCastInst(CastInst &I)
  {
    // TODO
    // Hint: ConstExpr::getCast()
    debug_func_name();
    debug("getNumOperands = "<<I.getNumOperands());
    Value* Op = I.getOperand(0);
    if(I.getNumOperands()>=1)
    {
      if(StateMap.count(Op))
      {
        if (StateMap[Op].isOverdefined())
        {
          StateMap[&I].markOverdefined();
        }
        else if (StateMap[Op].isUndefined() )
        {
          StateMap[&I].markUndefined();
        }

      }
      if (IsConstantValue(Op))
      {
        debug("Set to constant");
        Constant * ret = getConstant(Op);
        debug("ret = "<<*ret);
        ret = ConstantExpr::getCast(I.getOpcode(),ret,I.getType());
        debug("ret = "<<*ret);
        StateMap[dyn_cast<Value >(&I)].markConstant(ret);
        storeUsersInstructionToWorkList(I);
      }

    }
  }

  void visitInstruction(Instruction &I)
  {
    // TODO Fallback case
    debug_func_name();
    debug("getNumOperands = "<<I.getNumOperands());
    StateMap[dyn_cast<Value >(&I)].markOverdefined();
  }

private:
  /* Gets the current state of a Value. This method also lazily
   * initializes the state if there is no entry in the StateMap
   * for this Value yet. The initial value is CONSTANT for
   * Constants and UNDEFINED for everything else. */
  State &getValueState(Value *Val)
  {
    auto It = StateMap.insert({Val, State()});
    State &S = It.first->second;

    if (!It.second)
    {
      // Already in map, return existing state
      return S;
    }

    if (Constant *C = dyn_cast<Constant>(Val))
    {
      // Constants are constant...
      S.markConstant(C);
    }

    // Everything else is undefined (the default)
    return S;
  }

  /* Print the final result of the analysis. */
  void printResults(Function &F)
  {
    for (BasicBlock &BB : F)
    {
      for (Instruction &I : BB)
      {
        State S = getValueState(&I);
        errs() << I << "\n    -> " << S << "\n";
      }
    }
  }
  /* set all value to undefined */
  void setValueToUndefined(Function &F)
  {
    debug_func_name();
    for (BasicBlock &BB : F)
    {
      for (Instruction &I : BB)
      {
        State S = getValueState(&I);
        debug("set{" << I << "}->" << S);
      }
    }
  }
  /*store an Instruction to Worklist*/
  bool storeToWorklist(Value *Val)
  {
    debug_func_name();
    /*TODO: may be we should consider duplicates*/
    WorkList.push_back(Val);
    return true;
  }
  bool storeUsersInstructionToWorkList(Instruction &I)
  {
    debug_func_name();
    for (auto users : I.users())
    {
        storeToWorklist(users);
    }
  }
  bool storeAllInstructionToWorklist(Function &F)
  {
    for (BasicBlock &BB : F)
    {
      for (Instruction &I : BB)
      {
        storeToWorklist(&I);
      }
    }
    return true;
  }

  bool IsConstantValue(Value * Val)
  {

    debug_func_name();
    if (Constant *C = dyn_cast<Constant>(Val))
    {
      debug("originally constant");
      return true;
    }
    else if(StateMap[Val].isConstant() ){
      debug("constant in StateMap");

      return true;
    }
    else return false;
  }

  Constant * getConstant(Value* Val)
  {
    if (StateMap.count(Val))
    {
      if (StateMap[Val].isConstant())
      {
        return StateMap[Val].getConstant();
      }
    }
    else
    {
      return dyn_cast<Constant>(Val);
    }

  }
  void handleTwoOperandInstruction(Instruction &I, bool isComparison)
  {
    debug_func_name();
    debug("getNumOperands = "<<I.getNumOperands());
    if (I.getNumOperands() >= 2)
    {
      Value * FirstOp = I.getOperand(0);
      Value * SecondOp = I.getOperand(1);
      Constant * C1 = dyn_cast<Constant>(getConstant(FirstOp));
      Constant * C2 = dyn_cast<Constant>(getConstant(SecondOp));
      int FirstOp_status = s_CONSTANT;
      int SecondOp_status = s_CONSTANT;
      debug("Operand 1 @"<<FirstOp<<": "<<* FirstOp);
      debug("Operand 2 @"<<SecondOp<<": "<<* SecondOp);
      debug("Opcode = "<<I.getOpcode());
      
      if (StateMap.count(FirstOp))
      {
        debug("Found OP1!");
        if(StateMap[FirstOp].isOverdefined()) FirstOp_status = s_OVERDEFINED;
        else if(StateMap[FirstOp].isUndefined()) FirstOp_status = s_UNDEFINED;
      }
      if (StateMap.count(SecondOp))
      {
        debug("Found OP2!");
        if(StateMap[SecondOp].isOverdefined()) SecondOp_status = s_OVERDEFINED;
        else if(StateMap[SecondOp].isUndefined()) SecondOp_status = s_UNDEFINED;
      }
      
      if ( FirstOp_status == s_OVERDEFINED || SecondOp_status == s_OVERDEFINED)
      {
        debug("one of operand is overdefined");
        StateMap[&I].markOverdefined();
      }
      if ( FirstOp_status == s_UNDEFINED || SecondOp_status == s_UNDEFINED)
      {
        debug("one of operand is undefined");
        StateMap[&I].markUndefined();
      }
      else if (IsConstantValue(FirstOp)&&IsConstantValue(SecondOp))
      {
        debug("Both Operand are Constant, set to constant");
        // Constant * ret = calculateConstant(I);
        Constant * ret; 
        if(isComparison)
        {
          CmpInst *CI = dyn_cast<CmpInst>(&I);
          debug("Predicate="<<CI->getPredicate());
          ret = ConstantExpr::getCompare(CI->getPredicate(),C1,C2);
        }
        else
        {
          ret = ConstantExpr::get(I.getOpcode(),C1,C2);
        }
        debug("calculate ret = " << *ret);
        StateMap[dyn_cast<Value >(&I)].markConstant(ret);
        storeUsersInstructionToWorkList(I);

      }
      else
      {
        debug("default: do nothing");

      }
    }
  }


  void dumpWorklist()
  {
    int size = WorkList.size();
    debug(">>>dump Size: " << size);
    for (int i = 0; i < size; i++)
    {
      debug("["
            << "@" << WorkList[i] << ":" << *WorkList[i] << "]");
    }
    debug("<<<dump end");
  }
  void dumpStateMap()
  {
    debug_func_name();
    for (auto &It : StateMap)
    {

      debug("--");
      debug("Value @"<<It.first<<": "<<*(It.first));
      debug("State:"<<It.second);
      debug("--");
    }

  }

  // Map from Values to their current State
  DenseMap<Value *, State> StateMap;
  // Worklist of instructions that need to be (re)processed
  SmallVector<Value *, 64> WorkList;
};
}//namespace

// Pass registration
char ConstPropPass::ID = 0;
static RegisterPass<ConstPropPass> X("const-prop-pass", "Constant propagation pass");
