/*
 This file is part of DepQBF.

 DepQBF, a solver for quantified boolean formulae (QBF).        
 Copyright 2013, 2014, 2015, 2016, 2017 Florian Lonsing,
   Vienna University of Technology, Vienna, Austria.
 Copyright 2010, 2011, 2012 Florian Lonsing,
   Johannes Kepler University, Linz, Austria.
 Copyright 2012 Aina Niemetz,
   Johannes Kepler University, Linz, Austria.

 DepQBF is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or (at
 your option) any later version.

 DepQBF is distributed in the hope that it will be useful, but
 WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with DepQBF.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef QDPLL_H_INCLUDED
#define QDPLL_H_INCLUDED

#include <stdio.h>
#include <time.h>

#define walksat_pop(stack) stack[--stack ## _fill_pointer]
#define walksat_push(item, stack) stack[stack ## _fill_pointer++] = item
#define SAT_MAX_VARS    4000010
#define SAT_MAX_CLAUSES 20000000

// Define a data structure for a literal in the SAT problem.
struct SAT_lit {
    int clause_num;		//clause num, begin with 0
    int var_num;		//variable num, begin with 1
    int sense;			//is 1 for true literals, 0 for false literals.
};

struct SAT_clause {
    int clause_id;		//clause num, begin with 0
    int unsat_count;		//variable num, begin with 1
};

typedef struct SAT_lit SAT_lit;
typedef struct SAT_clause SAT_clause;


struct WalkSAT
{
    /*parameters of the instance*/
    int     num_vars;		//var index from 1 to num_vars
    int     num_clauses;	//clause index from 0 to num_clauses-1
    int		max_clause_len;
    int		min_clause_len;
    int		formula_len;//还得初始化为0
    double	avg_clause_len;
    double 	ratio;
    int aspiration_active;
    int SAT_restart;
    int qbf_new_init;

    int* clause_sat_to_qbf;//sat中子句编号  对应  qbf中子句编号  表
    int* clause_qbf_to_sat;

    /* literal arrays */
    //struct SAT_lit* var_lit[SAT_MAX_VARS];				//var_lit[i][j] means the j'th literal of var i.
    struct SAT_lit** var_lit;
    //int		var_lit_count[SAT_MAX_VARS];        //amount of literals of each var
    int* var_lit_count;
    //int var_type[SAT_MAX_VARS];//存储文字的类型，存在文字为1，任意文字为0
    int* var_type;
    //var_num存储的是变量，不是文字，只有和sense(0或1)一起用才是文字
    //struct SAT_lit* clause_lit[SAT_MAX_CLAUSES];		//clause_lit[i][j] means the j'th literal of clause i.
    struct SAT_lit** clause_lit;
    //int		clause_lit_count[SAT_MAX_CLAUSES]; 	// amount of literals in each clause
    int* clause_lit_count;
    //int		clause_var_count[SAT_MAX_CLAUSES]; 	// amount of literals in each clause
    int* clause_var_count;

    //struct SAT_lit* org_clause_lit[SAT_MAX_CLAUSES];		//clause_lit[i][j] means the j'th literal of clause i.
    struct SAT_lit** org_clause_lit;
    //int		org_clause_lit_count[SAT_MAX_CLAUSES]; 	// amount of literals in each clause
    int* org_clause_lit_count;
    int		simplify;//还得初始化为0

    /* Information about the variables. */
    //int     score[SAT_MAX_VARS];
    int* score;
    //int		time_stamp[SAT_MAX_VARS];
    int* time_stamp;
    //int		conf_change[SAT_MAX_VARS];
    int* conf_change;
    //int freq[SAT_MAX_VARS];
    int* freq;
    //int freq_sat_solution[SAT_MAX_VARS];//以往出现在SAT求解方案里面的次数
    int* freq_sat_solution;
    //int* var_neighbor[SAT_MAX_VARS];
    int** var_neighbor;
    //int		var_neighbor_count[SAT_MAX_VARS];
    int* var_neighbor_count;
    //int		pscore[MAX_VARS];
    //int		fix[SAT_MAX_VARS];
    int* fix;
    //int		dpll_fix[SAT_MAX_VARS];
    int* dpll_fix;


    /* Information about the clauses */
    //int     clause_weight[SAT_MAX_CLAUSES];
    //int     sat_count[SAT_MAX_CLAUSES];
    //int		sat_var[SAT_MAX_CLAUSES];
    int* clause_weight;
    int* sat_count;
    int* sat_var;
    //int		sat_var2[MAX_CLAUSES];

    //unsat clauses stack
    //int		unsat_stack[SAT_MAX_CLAUSES];		//store the unsat clause number
    int* unsat_stack;
    int		unsat_stack_fill_pointer;
    //int		index_in_unsat_stack[SAT_MAX_CLAUSES];//which position is a clause in the unsat_stack
    int* index_in_unsat_stack;

    int		this_try_best_unsat_stack_fill_pointer;

    //variables in unsat clauses
    //int		unsatvar_stack[SAT_MAX_VARS];
    int* unsatvar_stack;
    int		unsatvar_stack_fill_pointer;
    //int		index_in_unsatvar_stack[SAT_MAX_VARS];
    //int		unsat_app_count[SAT_MAX_VARS];		//a varible appears in how many unsat clauses
    int* index_in_unsatvar_stack;
    int* unsat_app_count;

    //configuration changed decreasing variables (score>0 and confchange=1)
    //int		goodvar_stack[SAT_MAX_VARS];
    int* goodvar_stack;
    int		goodvar_stack_fill_pointer;
    //int		already_in_goodvar_stack[SAT_MAX_VARS];
    int* already_in_goodvar_stack;

    //unit clauses preprocess
    //struct SAT_lit		unitclause_queue[SAT_MAX_VARS];
    struct SAT_lit* unitclause_queue;
    int		unitclause_queue_beg_pointer;//还得初始化为0
    int     unitclause_queue_end_pointer;//还得初始化为0
    //int     clause_delete[SAT_MAX_CLAUSES];
    int* clause_delete;

    /* Information about solution */
    //int             cur_soln[SAT_MAX_VARS];	//the current solution, with 1's for True variables, and 0's for False variables
    //int cur_soln1[SAT_MAX_VARS];
    int* cur_soln;	//the current solution, with 1's for True variables, and 0's for False variables
    int* cur_soln1;
    int use_fix_forall_init;

    //int qbf_org_bcp[SAT_MAX_VARS];
    int* qbf_org_bcp;
    int num_qbf_org_bcp;

    //cutoff
    int		max_tries;//还得初始化为10000
    int		tries;
    int		max_flips;//还得初始化为2000000000;
    int		step;

    double p_scale;
    double q_scale;
    int threshold;
    double ave_weight;
    int		delta_total_weight;
    double scale_ave;

    int satisfy_flag;

    //new unknown backtrack
    int use_new_unknown_backtrack;
    double hope_proportion;
    //int is_hope[SAT_MAX_VARS];//为1代表该变量是有希望的变量，后续可以进行翻转
    int* is_hope;
    //int clause_unsat_count[SAT_MAX_CLAUSES];//子句没有被满足的次数
    int* clause_unsat_count;
    //int local_search_stamp;//局部搜索的时间戳，每次调用sat求解器时重置,walksat->step就是这样的时间戳
    //int clause_unsat_stamp[SAT_MAX_CLAUSES];//子句加入unsat栈时的时间戳，下次在unsat栈中或结束后的时间戳 减去 加入unsat栈时的时间戳，加给子句未满足次数
    int* clause_unsat_stamp;
    //struct SAT_clause clause_unsat_count_sort[SAT_MAX_CLAUSES];
    struct SAT_clause* clause_unsat_count_sort;
    int num_clause_unsat_count_sort;
    //int num_through_var_decision[SAT_MAX_VARS];//backtrack时经过这个变量，做决策，的次数
    //int num_through_var_decision_sat[SAT_MAX_VARS];//backtrack时经过这个变量，做决策，并且局部搜索返回sat，的次数
    int* num_through_var_decision;
    int* num_through_var_decision_sat;
    int num_all_through_var_decision_sat;//backtrack时经过变量，做决策，求得sat的总数
    int num_all_through_var_decision;//backtrack时经过变量，做决策，求得sat的总数

    //采用unknown backtrack第二阶段的策略
    //假设一个有希望的变量a，如果反转a，可以使有希望的变量b逃离某个子句的严格限制（即那个子句中仅有b为true），那么a也应该有希望
    int use_second_unknown_backtrack_strategy;
    int num_use_second_unknown_backtrack_strategy;
    //int had_vis[SAT_MAX_CLAUSES];//该子句是否已经检查过
    int* had_vis;
    //int due_to_second_backtrack[SAT_MAX_VARS];
    int* due_to_second_backtrack;

    //用于实现unknown backtrack
    //int wr[SAT_MAX_VARS];//初始时维护的原因
    int* wr;
    int size_wr;//原因的长度
    //int in_wr[SAT_MAX_VARS];//判断变量是否在初始原因wr中
    int* in_wr;

    //用于QCDCL指导SAT局部搜索
    int** qsscore;//QCDCL指导sat局部搜索求解器的打分函数
    double tendency_factor;//倾向因子
    int unit_qsscore;//单位分数
    int tendency_qsscore;//倾向分数
    int max_qsscore;//分数上界
    int use_qcdcl_guide_sat;
    double avg_qsscore;//平均分数
    int reward_score;//奖励分数
    int big, small, equal;
    int avg_weight;
    int sum_weight;
    double reward_ratio;

    int* clause_to_variable_activity;
    //int* variable_activity;
    int record_activity;
    int use_variable_activity;
    int* temp_list;
    int num_temp_list;
    int* nesting;
    //int* is_collected;

    int *vc;
    int *vc_v;
    int num_vc = 0;

    int* record_priority;

    //hash
    int *hash;
    int is_repeat;
    int size_hash;
    int use_hash;

};

typedef struct QDPLL QDPLL;
typedef struct WalkSAT WalkSAT;
typedef int LitID;
typedef unsigned int VarID;
typedef unsigned int ConstraintID;
typedef unsigned int Nesting;
typedef unsigned int ClauseGroupID;

enum QDPLLResult
{
  QDPLL_RESULT_UNKNOWN = 0,
  QDPLL_RESULT_SAT = 10,
  QDPLL_RESULT_UNSAT = 20
};

typedef enum QDPLLResult QDPLLResult;

enum QDPLLQuantifierType
{
  QDPLL_QTYPE_EXISTS = -1,
  QDPLL_QTYPE_UNDEF = 0,
  QDPLL_QTYPE_FORALL = 1
};

typedef enum QDPLLQuantifierType QDPLLQuantifierType;

typedef int QDPLLAssignment;
#define QDPLL_ASSIGNMENT_FALSE -1
#define QDPLL_ASSIGNMENT_UNDEF 0
#define QDPLL_ASSIGNMENT_TRUE 1

/* Create and initialize solver instance. */
QDPLL *qdpll_create (void);

/* Delete and release all memory of solver instance. */
void qdpll_delete (QDPLL * qdpll);

/* Configure solver instance via configuration string. 
   Returns null pointer on success and error string otherwise.
*/
char *qdpll_configure (QDPLL * qdpll, char *configure_str);

/* Ensure var table size to be at least 'num'. */
void qdpll_adjust_vars (QDPLL * qdpll, VarID num);

/* Returns the nesting level of the current rightmost scope. */
Nesting qdpll_get_max_scope_nesting (QDPLL *qdpll);

/* Enforce the deletion of variables which have no occurrences left, and
   delete empty quantifier blocks. E.g. after 'qdpll_pop',
   'qdpll_delete_clause_group', a variable might not have any clauses left if
   all the clauses containing that variable were removed by the pop
   operation. However, the variable is still present in the prefix that was
   added by the user and 'is_var_declared' returns non-zero for these
   variables. The function 'qdpll_gc' cleans up the prefix and deletes
   variables which do not occur in the current formula and removes empty
   quantifier blocks. After 'qdpll_gc', is_var_declared' returns zero for
   variables which have been removed.
   NOTE: Calling 'qdpll_pop' or 'qdpll_delete_clause_group' does NOT delete
   variables and quantifier blocks, but only clauses.
   IMPORTANT NOTE: do NOT call 'qdpll_gc' unless you want to remove variables
   and quantifier blocks which you have previously added to the formula. */
void qdpll_gc (QDPLL *qdpll);

/* ------------ START: push/pop API functions ------------ */

/* IMPORTANT NOTE: calls of the push/pop API cannot be mixed with call of the
   clause group API. The solver will abort if a function of the clause group API
   is called after a call of either 'push' or 'pop', or vice versa. */

/* Decrease the current frame index by one and disable all clauses associated
   to the old, popped off frame. Returns either the old frame index which was
   popped off or zero if there is no frame to be popped off. */
unsigned int qdpll_pop (QDPLL *qdpll);

/* Increase the current frame index by one. Every clause added by 'qdpll_add'
   is attached to the current frame. The clauses attached to a frame will be
   discarded if the frame is popped off by 'qdpll_pop'. Returns the current
   frame index resulting from the push operation. */
unsigned int qdpll_push (QDPLL *qdpll);

/* ------------ END: push/pop API functions ------------ */

/* ------------ START: API functions for clause groups ------------ */

/* IMPORTANT NOTE: calls of the push/pop API cannot be mixed with call of the
   clause group API. The solver will abort if a function of the clause group API
   is called after a call of either 'push' or 'pop', or vice versa. */

/* Creates a new clause group and returns its ID. The returned ID is a handle
   of the created clause group and should be passed to API functions to
   manipulate clause groups. Initially, the newly created clause group is
   empty (i.e. it does not contain any clauses) and it is closed. To add
   clauses to a group, the group must be opened by 'open_clause_group'. Only
   one clause group can be open at a time. Clauses can be added to the
   currently open group as usual by calling 'qdpll_add'. To add clauses to a
   different group, the currently open group must be closed again by
   'close_clause_group' and the other group must be opened. Clause groups are
   activated at the time they are created and can be deactivated by calling
   'qdpll_deactivate_clause_group'. */
ClauseGroupID qdpll_new_clause_group (QDPLL *qdpll);

/* Delete the clause group with ID 'clause_group'. The group must be
   activated. The ID of the deleted group becomes invalid and must not be
   passed to the API functions anymore. All clauses in the deleted group are
   deleted from the formula. */
void qdpll_delete_clause_group (QDPLL *qdpll, ClauseGroupID clause_group);

/* Open the clause group with ID 'clause_group'. That group must not be open
   already and must be activated. Only one group can be open at a
   time. Clauses can be added to the currently open group as usual by calling
   'qdpll_add'. An open group will stay open across calls of
   e.g. 'qdpll_sat'. However, to add clauses to a another group, the currently
   open group must be closed again by 'close_clause_group' and the other group
   must be opened. */
void qdpll_open_clause_group (QDPLL *qdpll, ClauseGroupID clause_group);

/* Returns the ID of the currently open clause group, or NULL if no group is
   currently open. If there is currently no open group, then all clauses added
   via 'qdpll_add' will be permanently added to the formula and cannot be
   removed. */
ClauseGroupID qdpll_get_open_clause_group (QDPLL *qdpll);

/* Returns non-zero if and only if (1) a clause group with ID 'clause_group'
   has been created before and (2) the ID 'clause_group' was returned by
   'qdpll_new_clause_group' and (3) that clause group was not deleted by 
   calling 'qdpll_delete_clause_group'. */
int qdpll_exists_clause_group (QDPLL *qdpll, ClauseGroupID clause_group);

/* Close the clause group with ID 'clause_group'. That group must have been
   opened by a previous call of 'open_clause_group' and must be activated. */
void qdpll_close_clause_group (QDPLL *qdpll, ClauseGroupID clause_group);

/* Returns a zero-terminated array of clause group IDs representing those
   clause groups which contain clauses used by the solver to determine
   UNSATISFIABILITY by the most recent call of 'qdpll_sat'. That is, this
   function returns a subset of those clause groups which participiate in the
   resolution refutation of the current formula. This function can be called
   only if the most recent call of 'qdpll_sat' returned
   QDPLL_RESULT_UNSAT. The groups returned by this function are all activated.

   NOTE: the caller is responsible to release the memory of the array returned
   by this function. */
ClauseGroupID * qdpll_get_relevant_clause_groups (QDPLL * qdpll);

/* Activates all clauses in the group 'clause_group', which has been
   deactivated before by 'qdpll_deactivate_clause_group'. Clause groups are
   activated at the time they are created and can be deactivated by calling
   'qdpll_deactivate_clause_group'. */
void qdpll_activate_clause_group (QDPLL *qdpll, ClauseGroupID clause_group);

/* Deactivates all clauses in the group 'clause_group'. The ID of a
   deactivated group cannot be passed to any API functions except
   'qdpll_activate_clause_group' and 'qdpll_exists_clause_group'. Clause
   groups are activated at the time they are created. When calling
   'qdpll_sat', clauses in deactivated groups are ignored. Thus deactivating a
   clause group amounts to temporarily deleting these groups from the
   formula. However, unlike 'qdpll_delete_clause_group' which permanently
   deletes the clauses in a group, deactivated groups can be activated again
   by calling 'qdpll_activate_clause_group'. This adds the formerly
   deactivated clauses back to the formula. */
void qdpll_deactivate_clause_group (QDPLL *qdpll, ClauseGroupID clause_group);

/* ------------ END: API functions for clause groups ------------ */

/* Open a new scope at the right end of the quantifier prefix, where variables
   can be added by 'qdpll_add'. The opened scope must be closed by adding '0'
   via 'qdpll_add'. Returns the nesting of the added scope, which should be
   used as a handle of this scope, and which can safely be passed to
   'qdpll_add_var_to_scope'.  NOTE: will fail if there is an opened scope
   already.  */
Nesting qdpll_new_scope (QDPLL * qdpll, QDPLLQuantifierType qtype);

/* Open a new scope at nesting level 'nesting >= 1' with quantifier type
   'qtype'. Variables can be added to the scope opened by the most recent call
   of this function by 'qdpll_add' (similar to 'qdpll_new_scope'). The opened
   scope must be closed by adding '0' via 'qdpll_add'. Returns the nesting of
   the added scope, which should be used as a handle of this scope, and which
   can safely be passed to 'qdpll_add_var_to_scope'.
   NOTE: the run time of this function is linear in the length of quantifier prefix. */
Nesting qdpll_new_scope_at_nesting (QDPLL * qdpll, QDPLLQuantifierType qtype, Nesting nesting);

/* Add a new variable with ID 'id' to the scope with nesting level
   'nesting'. The scope must exist, i.e. it must have been added by either
   'qdpll_new_scope' or 'qdpll_new_scope_at_nesting'. The value of the parameter
   'nesting' of this function should be a value returned by a previous call of
   'qdpll_new_scope' or 'qdpll_new_scope_at_nesting'. In any case, it must be
   smaller than or equal to the return value of 'qdpll_get_max_scope_nesting'. */
void qdpll_add_var_to_scope (QDPLL *qdpll, VarID id, Nesting nesting);

/* This function is deprecated. */
int qdpll_has_var_active_occs (QDPLL *qdpll, VarID id);

/* Add variables or literals to clause or opened scope. Scopes are opened by
   either 'qdpll_new_scope' or 'qdpll_new_scope_at_nesting'. If scope is
   opened, then 'id' is interpreted as a variable ID, otherwise 'id' is
   interpreted as a literal. If 'id == 0' then the clause/scope is closed.

   IMPORTANT NOTE: added clauses are associated to the current frame. If
   'qdpll_push' has NOT been called before then the added clauses are
   permanently added to the formula. Otherwise, they are added to the current
   frame and can be remove from the formula by calling 'qdpll_push'.

   Similarly, added clauses are associated to the clause group which has been
   opened before by calling 'qdpll_open_clause_group', if any. If
   'qdpll_open_clause_group' has NOT been called before then the added clauses
   are permanently added to the formula.

   NOTE: function will fail if a scope is opened and 'id' is negative.

   NOTE: if a clause containing literals of undeclared variables is added by
   'qdpll_add' then these literals by default will be existentially quantified
   and put in the leftmost scope. That is, the variable of these literals is
   interpreted as a free variable. See also function 'qdpll_is_var_declared'
   below. */
void qdpll_add (QDPLL * qdpll, LitID id);

/* Solve the formula. */
QDPLLResult qdpll_sat (QDPLL * qdpll);

/* Get assignment of variable. */
QDPLLAssignment qdpll_get_value (QDPLL * qdpll, VarID id);

/* Print QBF to 'out' using QDIMACS format. */
void qdpll_print (QDPLL * qdpll, FILE * out);

/* Print QDIMACS-compliant output. */
void qdpll_print_qdimacs_output (QDPLL * qdpll);

/* Initialize the current dependency manager. The dependency scheme is
   computed with respect to the clauses added by 'qdpll_add'. If the
   dependency scheme has been computed already then calling this function has
   no effect. The dependency manager can be reset and re-initialized by calling
   'qdpll_reset_deps' and then 'qdpll_init_deps'.*/
void qdpll_init_deps (QDPLL * qdpll);

/* Reset the current dependency manager. Dependencies can be re-initialized by
   calling 'qdpll_deps_init' or 'qdpll_sat'. */
void qdpll_reset_deps (QDPLL * qdpll);

/* Returns non-zero if variable 'id2' depends on variable 'id1', 
   i.e. if id1 < id2, with respect to the current dependency scheme. */
int qdpll_var_depends (QDPLL * qdpll, VarID id1, VarID id2);

/* Print zero-terminated list of dependencies for 
   given variable to 'stdout'. */
void qdpll_print_deps (QDPLL * qdpll, VarID id);

/* Return largest declared variable ID. */
VarID qdpll_get_max_declared_var_id (QDPLL * qdpll);

/* Returns non-zero if and only if (1) a variable with ID 'id' has been added
   to the solver by a previous call of 'qdpll_add' or
   'qdpll_add_var_to_scope'. For example, the function can be used to check if
   the variable of each literal in a clause to be added has been declared
   already. If not, then it can be declared by 'qdpll_add_var_to_scope' and
   put in the right scope.  NOTE: if a clause containing literals of
   undeclared variables is added by 'qdpll_add' then these literals by default
   will be existentially quantified and put in the leftmost scope. */
int qdpll_is_var_declared (QDPLL * qdpll, VarID id);

/* Returns the nesting level 'level' in the range '1 <= level <=
   qdpll_get_max_scope_nesting()' of the previously declared variable with ID
   'id'. Returns 0 if the variable with ID 'id' is free, i.e. not explicitly
   associated to a quantifier block. Fails if 'id' does not correspond to a
   declared variable, which should be checked with function
   'qdpll_is_var_declared()' before. */
Nesting qdpll_get_nesting_of_var (QDPLL * qdpll, VarID id);

/* Returns the quantifier type (i.e. either QDPLL_QTYPE_EXISTS or
   QDPLL_QTYPE_FORALL) of the scope at nesting level 'nesting'. 
   Returns zero if there is no scope with nesting level 'nesting'. */
QDPLLQuantifierType qdpll_get_scope_type (QDPLL *qdpll, Nesting nesting);

/* Dump dependency graph to 'stdout' in DOT format. */
void qdpll_dump_dep_graph (QDPLL * qdpll);

/* Print statistics to 'stderr'. */
void qdpll_print_stats (QDPLL * qdpll);

/* Reset internal solver state, keep clauses and variables. */
void qdpll_reset (QDPLL * qdpll);

/* Reset collected statistics. */
void qdpll_reset_stats (QDPLL * qdpll);

/* Discard all learned constraints. */
void qdpll_reset_learned_constraints (QDPLL * qdpll);

/* Assign a variable as assumption. A later call of 'qdpll_sat(...)' solves
   the formula under the assumptions specified before. If 'id' is negative
   then variable with ID '-id' will be assigned false, otherwise variable with
   ID 'id' will be assigned true. */
void qdpll_assume (QDPLL * qdpll, LitID id);

/* Returns a zero-terminated array of LitIDs of variables which can safely be
   assigned as assumptions by function 'qdpll_assume'. The array may contain
   both existential (positive LitIDs) and universal variables (negative
   LitIDs) which are not necessarily from the leftmost quantifier set in the
   prefix.

   NOTE: the caller is responsible to release the memory of the array returned
   by this function. */
LitID * qdpll_get_assumption_candidates (QDPLL * qdpll);

/* Returns a zero-terminated array of LitIDs representing those assumptions
   (passed to the solver using 'qdpll_assume()') which were used by the solver to
   determine (un)satisfiability by the most recent call of 'qdpll_sat'.

   NOTE: the caller is responsible to release the memory of the array returned
   by this function. */
LitID * qdpll_get_relevant_assumptions (QDPLL * qdpll);

#endif
