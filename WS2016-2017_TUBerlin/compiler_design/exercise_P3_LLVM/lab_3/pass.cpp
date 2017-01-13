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

		



		/*star from here*/

		vector<unsigned long int> BB_set;
		BasicBlock* first_BB =F.begin() ;
		vector<string> all_variables;
		map<BasicBlock*,bool> no_change_flag_set;
		map<BasicBlock*,vector<string>> in_set;
		map<BasicBlock*,vector<string>> out_set;
		map<BasicBlock*,vector<string>> load_set;
		map<BasicBlock*,vector<string>> store_set;


		/*0. get load set and store set of every block*/
		/*1. the in set of first basic block (BB)= parameter, out set of first BB =  in set+store set*/
		/*2. scan for variables vector*/
		/*3. from the second basic block, in set = all variable, out set = all variables*/
		/*4. loop
		 	1) for each BB, in set = intersection of all parent().out_set, in no parent, skip this
		 	2) for each BB, new out set = in set + store set
		 	3) if new out set != out set, no_change_flag_set = false
			4) if all no_change_flag_set == true, break*/
		/*5. for each Instrument, inner in set = in set + store, compare the load and inner in set*/

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
				/*2. scan for all variables*/
				all_variables.push_back(PtrOp->getName());

			    }
			}
			else if (LoadInst *LI = dyn_cast<LoadInst>(&I)) {
			    Value *PtrOp = LI->getPointerOperand(); // Load target
			    if (PtrOp->hasName()) {
				DebugLoc Loc = LI->getDebugLoc();
				errs() << "load Variable (" << PtrOp->getName()
				    << ") written on line " << Loc.getLine() << "\n";
				load_set[&BB].push_back(PtrOp->getName());

			    }
			}

		    }

		}
		
		/* 1.4 in_set['first BB']=(function's parameters)*/
		/* 1.5 out_set['first BasicBlock']=in_set+ store set*/
		/*find out first Basic Block*/
		
		for (BasicBlock &BB : F)
		{
		    BB_set.push_back((unsigned long int)&BB);
		}

		sort(BB_set.begin(),BB_set.end());

		/*dump*/
		errs()<<"\ndump BB_set:[";
		for (int i = 0;i<BB_set.size();i++)
		{
		    errs()<<BB_set[i]<<" ";
		}
		errs()<<"]\n";
		/*dump end*/
		for (Argument &a: F.getArgumentList())
		{
		    in_set[first_BB].push_back(a.getName());
		}
		
		errs()<<"first_BB:"<<first_BB<<"\n";
		//out set = in set
		out_set[first_BB] = in_set[first_BB];
		//out set += stroe set
		std::copy(store_set[first_BB].begin(),store_set[first_BB].end(),std::back_inserter(out_set[first_BB]));

		/*2. scan for all variables*/
		for (BasicBlock &BB : F)
		{
		    for (Instruction &I : BB) {
			if (StoreInst *SI = dyn_cast<StoreInst>(&I)) {
			    Value *PtrOp = SI->getPointerOperand(); // Store target
			    if (PtrOp->hasName()) {
				DebugLoc Loc = SI->getDebugLoc();
				all_variables.push_back(PtrOp->getName());

			    }
			}
			else if (LoadInst *LI = dyn_cast<LoadInst>(&I)) {
			    Value *PtrOp = LI->getPointerOperand(); // Load target
			    if (PtrOp->hasName()) {
				DebugLoc Loc = LI->getDebugLoc();
				errs() << "load Variable (" << PtrOp->getName()
				    << ") written on line " << Loc.getLine() << "\n";
				all_variables.push_back(PtrOp->getName());
				load_set[&BB].push_back(PtrOp->getName());

			    }
			}
			else if (AllocaInst *AI = dyn_cast<AllocaInst>(&I)) {
			    if (AI->hasName()) {
				all_variables.push_back(AI->getName());

			    }
			}

		    }

		}
		// all variables = all store variables + parameters
		std::copy(in_set[first_BB].begin(),in_set[first_BB].end(),std::back_inserter(all_variables));

		//clean duplicate items
		std::sort(all_variables.begin(),all_variables.end());
		all_variables.erase(std::unique(all_variables.begin(),all_variables.end()),all_variables.end());
		errs() << "all variables: " ; 
		dump_string_vector(all_variables);
		errs() << "\n";

		/*3. from the second basic block, in set = all variable, out set = all variables*/
		for (int i = 0; i< BB_set.size();i++)
		{
		    if (((BasicBlock*)BB_set[i] == first_BB)) continue;
		    in_set[(BasicBlock*)BB_set[i]] = all_variables;
		    out_set[(BasicBlock*)BB_set[i]] = all_variables;
		    errs() << "in_set["<< (BasicBlock*)BB_set[i]<<"]="; 
		    dump_string_vector(in_set[(BasicBlock*)BB_set[i]]);
		    errs() << "\n";
		    errs() << "out_set["<< (BasicBlock*)BB_set[i]<<"]="; 
		    dump_string_vector(out_set[(BasicBlock*)BB_set[i]]);
		    errs() << "\n";

		}

		


		/*4. loop
		 	1) for each BB, in set = intersection of all parent().out_set, in no parent, skip this
		 	2) for each BB, new out set = in set + store set
		 	3) if new out set != out set, no_change_flag_set = false
			4) if all no_change_flag_set == true, break*/

		//TODO:!!change it to an infinete loop
		for (int i = 0;i<3;i++)
		{
		    errs()<<"\ntesting predecessors!<"<<i<<">\n";
		    for (BasicBlock &BB : F)
		    {
			vector<string> pred_outs;
			vector<string> tmp_pred_outs;
			vector<string>::iterator pred_outs_it;

		    	errs()<<"\n>>>>>>>>>>>>>>>>>>>>>>>\n";
		    	errs()<<"BB("<<&BB<<"):";
			if ( pred_begin(&BB)== pred_end(&BB))
			{
			    errs()<<"skip in/out analysis for no parent block!\n";
			    continue;
			}
			if ( &BB == first_BB)
			{
			    errs()<<"skip in/out analysis for  first block!\n";
			    continue;
			}

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
				std::sort(pred_outs.begin(),pred_outs.end());
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

			    /*
			       ugly method of intersection
			    */

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
			//in set = intersection of (all parents.out + in set)

			
			errs()<<"before intersection in & pred_outs!!!\n";
			errs()<<"tmp_pred_outs:";
			dump_string_vector(tmp_pred_outs);
			errs()<<"\n";
			errs()<<"pred_outs:";
			dump_string_vector(pred_outs);
			errs()<<"\n";
			errs()<<"in_set["<<&BB<<"]:";
			dump_string_vector(in_set[&BB]);
			errs()<<"\n";
			for (int i = 0; i<pred_outs.size(); i++)
			{

			    if (std::find(in_set[&BB].begin(),in_set[&BB].end(),pred_outs[i]) == in_set[&BB].end())
			    {
				errs()<<"!!!cannot find element in this outset";
				//if cannot find a value of tmp vector from new parent's out_set, remove it
				tmp_pred_outs.erase(std::remove(tmp_pred_outs.begin(),tmp_pred_outs.end(),pred_outs[i]),tmp_pred_outs.end());
				errs()<<">>>>";
				dump_string_vector(tmp_pred_outs);
				errs()<<"\n";

			    }
			}
			pred_outs = tmp_pred_outs;
			in_set[&BB] = pred_outs;
			errs()<<"AFTER intersection in & pred_outs!!!\n";
			errs()<<"tmp_pred_outs:";
			dump_string_vector(tmp_pred_outs);
			errs()<<"\n";
			errs()<<"pred_outs:";
			dump_string_vector(pred_outs);
			errs()<<"\n";
			errs()<<"in_set[&BB]:";
			dump_string_vector(in_set[&BB]);
			errs()<<"\n";
			//make new out set
			errs()<<"\n\t+ store_set :";
			std::copy(store_set[&BB].begin(),store_set[&BB].end(),std::back_inserter(pred_outs));
			std::sort(pred_outs.begin(),pred_outs.end());
			dump_string_vector(pred_outs);
			errs()<<"\n\t- duplicates:";
			pred_outs.erase(std::unique(pred_outs.begin(),pred_outs.end()),pred_outs.end());
			dump_string_vector(pred_outs);
			errs()<<"\n old out_set: ";
			dump_string_vector(out_set[&BB]);
			errs()<<"\n";
			if (!compare_string_vectors(pred_outs,out_set[&BB]))
			{
			    errs()<<"out_set and pred_outs not match!\n";
			    no_change_flag_set[&BB] = false;
			    out_set[&BB] = pred_outs;
			    errs()<<"new out set =";
			    dump_string_vector(out_set[&BB]);
			    errs()<<"\n";

			}
			else
			{
			    errs()<<"out_set and pred_outs MATCHED!!\n";
			    no_change_flag_set[&BB] = true;
			}

		    }
		    
		    errs()<<"end of testing predecessors!<"<<i<<">\n";

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


		/*2. for each load instrument, find that variable in in_set, if canno find, print error*/
		/*5. for each Instrument, inner in set = in set + store, compare the load and inner in set*/

		for (BasicBlock &BB : F)
		{
		    vector<string> inner_set = in_set[&BB];
		    errs()<<"original inner_set:";
		    dump_string_vector(inner_set);
		    errs()<<"\n";
		    for (Instruction &I : BB) {


			if (StoreInst *SI = dyn_cast<StoreInst>(&I)) {
			    Value *PtrOp = SI->getPointerOperand(); // Store target
			    if (PtrOp->hasName()) {
				DebugLoc Loc = SI->getDebugLoc();
				inner_set.push_back(PtrOp->getName());
				errs()<<"update inner_set:";
				dump_string_vector(inner_set);
				errs()<<"\n";

			    }
			}
			else if (LoadInst *LI = dyn_cast<LoadInst>(&I)) {
			    Value *PtrOp = LI->getPointerOperand(); // Load target
			    if (PtrOp->hasName()) {
				DebugLoc Loc = LI->getDebugLoc();
				if(std::find(inner_set.begin(),inner_set.end(),PtrOp->getName()) == inner_set.end())
				{
				    errs() << "uninit Variable (" << PtrOp->getName()
					<< ") written on line " << Loc.getLine() << "@("<<&BB<<")\n";
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
