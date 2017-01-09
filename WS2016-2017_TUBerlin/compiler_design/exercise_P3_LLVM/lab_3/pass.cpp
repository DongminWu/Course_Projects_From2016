/* First-Name Last-Name Matr-No */

/* TODO: Add a short explanation of your algorithm here.
 * E.g., if you use iterative data-flow analysis, write down
 * the used gen/kill sets and flow-equations here. */

#include <utility>
#include <algorithm>


// Include some headers that might be useful
#include <llvm/Pass.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Instruction.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/CFG.h>
#include <llvm/IR/InstIterator.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/ValueMap.h>
#include <llvm/ADT/BitVector.h>
#include <llvm/ADT/DenseSet.h>
#include <llvm/Support/raw_ostream.h>

using namespace llvm;
using std::string;
using std::map;
using std::vector;
using std::make_pair;


namespace {

    class DefinitionPass : public FunctionPass {
	public:
	    static char ID;
	    DefinitionPass() : FunctionPass(ID) {}

	    virtual void getAnalysisUsage(AnalysisUsage &au) const {
		au.setPreservesAll();
	    }

	    void dump_string_vector(vector<string> &v)
	    {
		errs()<<"[";
		for (int i =0; i<v.size();i++)
		{
		    errs()<<v[i]<<" ";
		}
		errs()<<"]";
		return;

	    }

	    bool compare_string_vectors(vector<string> &v1, vector<string> &v2)
	    {
		bool ret = false;
		if (v1.size() != v2.size())
		    return false;
		if (v1.size() < v2.size())
		    ret = std::equal(v1.begin(),v1.end(),v2.begin());
		else
		    ret = std::equal(v2.begin(),v2.end(),v1.begin());
		return ret;
	    }


	    virtual bool runOnFunction(Function &F) {
		/*
		// Example: Print all stores and where they occur
		for (BasicBlock &BB : F) {
		    for (Instruction &I : BB) {
			if (StoreInst *SI = dyn_cast<StoreInst>(&I)) {
			    Value *PtrOp = SI->getPointerOperand(); // Store target
			    if (PtrOp->hasName()) {
				DebugLoc Loc = SI->getDebugLoc();
				errs() << "Variable " << PtrOp->getName()
				    << " written on line " << Loc.getLine() << "\n";
			    }
			}
		    }
		}

		// We did not modify the function
		return false;
		*/

		/*CPP testing*/
		vector<string> string_vector;
		string_vector.push_back("a");
		string_vector.push_back("b");
		string_vector.push_back("c");
		string_vector.push_back("c");
		dump_string_vector(string_vector);
		



		/*star from here*/

		vector<unsigned long int> BB_set;
		map<BasicBlock*,bool> no_change_flag_set;
		map<BasicBlock*,vector<string>> in_set;
		map<BasicBlock*,vector<string>> out_set;
		map<BasicBlock*,vector<string>> load_set;
		map<BasicBlock*,vector<string>> store_set;

		/*0. init*/

		for (BasicBlock &BB : F)
		{
		    vector<string> empty;
		    no_change_flag_set.insert(make_pair(&BB,false));
		    in_set.insert(make_pair(&BB,empty));
		    out_set.insert(make_pair(&BB,empty));
		    load_set.insert(make_pair(&BB,empty));
		    store_set.insert(make_pair(&BB,empty));
		}

		/*1. computing the in set and out set of each BasicBlock*/


		/* 1.1 At first, all of the in sets should be empty*/
		/* 1.2 computing the store set and load set of each BasicBlock*/
		/* 1.3 all out set = store set*/
		for (BasicBlock &BB : F)
		{
		    for (Instruction &I : BB) {
			if (StoreInst *SI = dyn_cast<StoreInst>(&I)) {
			    Value *PtrOp = SI->getPointerOperand(); // Store target
			    if (PtrOp->hasName()) {
				DebugLoc Loc = SI->getDebugLoc();
				errs() << "store Variable (" << PtrOp->getName()
				    << ") written on line " << Loc.getLine() << "\n";
				store_set[&BB].push_back(PtrOp->getName());
				out_set[&BB].push_back(PtrOp->getName());

			    }
			}
			else if (LoadInst *LI = dyn_cast<LoadInst>(&I)) {
			    Value *PtrOp = LI->getPointerOperand(); // Store target
			    if (PtrOp->hasName()) {
				DebugLoc Loc = LI->getDebugLoc();
				errs() << "load Variable (" << PtrOp->getName()
				    << ") written on line " << Loc.getLine() << "\n";
				load_set[&BB].push_back(PtrOp->getName());

			    }
			}

		    }

		}
		
		/* 1.4 out_set['Entry']=(function's parameters)*/
		/* 1.5 in_set['first BasicBlock']=out_set['Entry']=(function's parameters)*/
		/*find out first Basic Block*/
		
		for (BasicBlock &BB : F)
		{
		    BB_set.push_back((unsigned long int)&BB);
		}

		sort(BB_set.begin(),BB_set.end());

		/*dump*/
		errs()<<"\ndump BB_Set:[";
		for (int i = 0;i<BB_set.size();i++)
		{
		    errs()<<BB_set[i]<<" ";
		}
		errs()<<"]\n";
		/*dump end*/

		if (BB_set.size() > 0)
		{
		    for (Argument &a: F.getArgumentList())
		    {
			store_set[(BasicBlock*)BB_set[0]].push_back(a.getName());
			in_set[(BasicBlock*)BB_set[0]].push_back(a.getName());
			out_set[(BasicBlock*)BB_set[0]].push_back(a.getName());
		    }
		    //TODO:may not use insert
		}

		/* 1.6 loop: 	1) in_set = parents().out_set with intersection
		 		2) in_set + store_set = new_out_set
		 		3) if new_out_set = out_set: no_change = true
		 		4) 	else: no_change = false
		 		4.1)		out_set = new_out_set
		       until:	all in no_change_set = true*/


		bool all_no_change_flag = false; 
		for (int i = 0;  i< 4;i++)
		{
		    errs()<<"\ntesting predecessors!<"<<i<<">\n";
		    for (BasicBlock &BB : F)
		    {
			vector<string> pred_outs;
			vector<string> tmp_pred_outs;

		    	errs()<<"\n>>>>>>>>>>>>>>>>>>>>>>>\n";
		    	errs()<<"BB("<<&BB<<"):";
			//copy first out to tmp vector

			bool flag = false;
			for (auto it = pred_begin(&BB), et = pred_end(&BB); it != et; ++it)
			{
			    errs()<<*it<<" ";
			    if (flag == false)
			    {
				std::copy(out_set[*it].begin(),out_set[*it].end(),std::back_inserter(pred_outs));
				std::copy(out_set[*it].begin(),out_set[*it].end(),std::back_inserter(tmp_pred_outs));
				pred_outs = out_set[*it];
				tmp_pred_outs = out_set[*it];
				errs()<<"tmp_pred_outs:";
				dump_string_vector(tmp_pred_outs);
				errs()<<"\n";
				flag = true;
			    }
			    if (pred_outs.size()== 0)
			    {

			    	errs()<<"\nempty pred_outs @first";
				tmp_pred_outs.clear();
				break;
			    }
			    if (out_set[*it].size()>0) //out_set is not empty
			    {
			    	errs()<<"\nout_set("<<*it<<"):";
				dump_string_vector(out_set[*it]);
			    	errs()<<"\n====pred_out.size("<<pred_outs.size()<<")======\n";
			    	errs()<<"====out_set[*it].size("<<out_set[*it].size()<<")======\n";
				
				for (int i = 0; i<pred_outs.size();i++)
				{
				    errs()<<"("<<i<<"):";
				    errs()<<"tmp_pred_outs:";
				    dump_string_vector(tmp_pred_outs);
				    errs()<<"\n";
				    errs()<<"pred_outs:";
				    dump_string_vector(pred_outs);
				    errs()<<"\n";
				    if (std::find(out_set[*it].begin(),out_set[*it].end(),pred_outs[i]) == out_set[*it].end())
				    {
					errs()<<"!!!cannot find element in this outset";
					//if cannot find a value of tmp vector from new parent's out_set, remove it
					tmp_pred_outs.erase(std::remove(tmp_pred_outs.begin(),tmp_pred_outs.end(),pred_outs[i]),tmp_pred_outs.end());
					errs()<<">>>>";
				    	dump_string_vector(tmp_pred_outs);
					errs()<<"\n";
					
				    }
				}
			    	errs()<<"\n============\n";

			    }
			    else 
			    {
			    	errs()<<"\nempty pred_outs";
				pred_outs.clear();
				tmp_pred_outs.clear();
			    }
			}
			//pred_outs.erase(pred_outs.begin(),pred_outs.end());
			//std::copy(tmp_pred_outs.begin(),tmp_pred_outs.end(),std::back_inserter(pred_outs));
			pred_outs = tmp_pred_outs;
			errs()<<"tmp_pred_outs:";
			dump_string_vector(tmp_pred_outs);
			errs()<<"\n";
			errs()<<"pred_outs:";
			dump_string_vector(pred_outs);
			errs()<<"\n";
			//copy to in set
			std::copy(pred_outs.begin(),pred_outs.end(),std::back_inserter(in_set[&BB]));
			std::sort(in_set[&BB].begin(),in_set[&BB].end());
			in_set[&BB].erase(std::unique(in_set[&BB].begin(),in_set[&BB].end()),in_set[&BB].end());
			//make new out set
			errs()<<"\n\t+ store_set :";
			std::copy(store_set[&BB].begin(),store_set[&BB].end(),std::back_inserter(pred_outs));
			std::sort(pred_outs.begin(),pred_outs.end());
			dump_string_vector(pred_outs);
			errs()<<"\n\t- duplicates:";
			pred_outs.erase(std::unique(pred_outs.begin(),pred_outs.end()),pred_outs.end());
			dump_string_vector(pred_outs);
			errs()<<"\nout_set: ";
			dump_string_vector(out_set[&BB]);
			errs()<<"\n";
			if (!compare_string_vectors(pred_outs,out_set[&BB]))
			{
			    errs()<<"out_set and pred_outs not match!\n";
			    no_change_flag_set[&BB] = false;
			    std::copy(pred_outs.begin(),pred_outs.end(),std::back_inserter(out_set[&BB]));
			    std::sort(out_set[&BB].begin(),out_set[&BB].end());
			    out_set[&BB].erase(std::unique(out_set[&BB].begin(),out_set[&BB].end()),out_set[&BB].end());
			}
			else
			{
			    no_change_flag_set[&BB] = true;
			}

		    }
		    errs()<<"end of testing predecessors!<"<<i<<">\n";
		    all_no_change_flag = true;
		    for (BasicBlock &BB : F)
		    {
			if (no_change_flag_set[&BB] == false)
			{
			    all_no_change_flag = false;
			    break;
			}
		    }
		    for (BasicBlock &BB : F)
		    {
			errs()<<no_change_flag_set[&BB]<<" ";
		    }
		    errs()<<"\n";
		    if(all_no_change_flag == true)
			break;

		}

		


		//for testing: dump
		for (BasicBlock &BB : F)
		{
		    errs()<<"BasicBlock:("<<&BB<<"):\n";
		    errs()<<"store_set:";
		    if ((!store_set.empty() )&& (store_set.find(&BB) != store_set.end()))
		    {
			dump_string_vector(store_set[&BB]);
		    }
		    errs()<<"\n";
		    errs()<<"load_set:";
		    if ((!load_set.empty() )&& (load_set.find(&BB) != load_set.end()))
		    {
			dump_string_vector(load_set[&BB]);
		    }
		    errs()<<"\n";
		    errs()<<"in_set:";
		    if ((!in_set.empty() )&& (in_set.find(&BB) != in_set.end()))
		    {
			dump_string_vector(in_set[&BB]);

		    }
		    errs()<<"\n";
		    errs()<<"out_set:";
		    if ((!out_set.empty() )&& (out_set.find(&BB) != out_set.end()))
		    {
			dump_string_vector(out_set[&BB]);
		    }
		    errs()<<"\n";
		    errs()<<"----------------------------\n";

		}


		/*2. for each load variables, find that variable in in_set, if canno find, print error*/

		for (BasicBlock &BB : F)
		{
		    for (Instruction &I : BB) {
			if (LoadInst *LI = dyn_cast<LoadInst>(&I)) {
			    Value *PtrOp = LI->getPointerOperand(); // Store target
			    if (PtrOp->hasName()) {
				DebugLoc Loc = LI->getDebugLoc();
				if(std::find(in_set[&BB].begin(),in_set[&BB].end(),PtrOp->getName()) == in_set[&BB].end())
				{
				    errs() << "uninit Variable (" << PtrOp->getName()
					<< ") written on line " << Loc.getLine() << "\n";
				}
			    }
			}
		    }
		}



		


	    }
    };

    class FixingPass : public FunctionPass {
	public:
	    static char ID;
	    FixingPass() : FunctionPass(ID) {}

	    virtual void getAnalysisUsage(AnalysisUsage &au) const {
		au.setPreservesCFG();
	    }

	    virtual bool runOnFunction(Function &F) {
		// TODO

		// The function was modified
		return true;
	    }
    };

} // namespace


char DefinitionPass::ID = 0;
char FixingPass::ID = 1;

// Pass registrations
static RegisterPass<DefinitionPass> X("def-pass", "Uninitialized variable pass");
static RegisterPass<FixingPass> Y("fix-pass", "Fixing initialization pass");
