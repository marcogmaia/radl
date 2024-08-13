/*
A* Algorithm Implementation using STL is
Copyright (C)2001-2005 Justin Heyes-Jones

Permission is given by the author to freely redistribute and
include this code in any program as long as this credit is
given where due.

  COVERED CODE IS PROVIDED UNDER THIS LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED,
  INCLUDING, WITHOUT LIMITATION, WARRANTIES THAT THE COVERED CODE
  IS FREE OF DEFECTS, MERCHANTABLE, FIT FOR A PARTICULAR PURPOSE
  OR NON-INFRINGING. THE ENTIRE RISK AS TO THE QUALITY AND
  PERFORMANCE OF THE COVERED CODE IS WITH YOU. SHOULD ANY COVERED
  CODE PROVE DEFECTIVE IN ANY RESPECT, YOU (NOT THE INITIAL
  DEVELOPER OR ANY OTHER CONTRIBUTOR) ASSUME THE COST OF ANY
  NECESSARY SERVICING, REPAIR OR CORRECTION. THIS DISCLAIMER OF
  WARRANTY CONSTITUTES AN ESSENTIAL PART OF THIS LICENSE. NO USE
  OF ANY COVERED CODE IS AUTHORIZED HEREUNDER EXCEPT UNDER
  THIS DISCLAIMER.

  Use at your own risk!

*/

#pragma once
#include <cassert>

// stl includes
#include <algorithm>
#include <cfloat>
#include <vector>

// fast fixed size memory allocator, used for fast node memory management
#include "fsa.hpp"

// Fixed size memory allocator can be disabled to compare performance
// Uses std new and delete instead if you turn it off
#define USE_FSA_MEMORY 1

// disable warning that debugging information has lines that are truncated
// occurs in stl headers
#if defined(WIN32) && defined(_WINDOWS)
#pragma warning(disable : 4786)
#endif

template <class T> class AStarState;

// The AStar search class. UserState is the users state space type
template <class UserState> class AStarSearch {
public: // data
  enum {
    kSearchStateNotInitialised,
    kSearchStateSearching,
    kSearchStateSucceeded,
    kSearchStateFailed,
    kSearchStateOutOfMemory,
    kSearchStateInvalid
  };

  // A node represents a possible state in the search
  // The user provided state type is included inside this type
  class Node {
  public:
    Node *parent = nullptr; // used during the search to record the parent
                            // of successor nodes
    Node *child = nullptr;  // used after the search for the application to
                            // view the search in reverse

    float g = 0.F; // cost of this node + it's predecessors
    float h = 0.F; // heuristic estimate of distance to goal
    float f = 0.F; // sum of cumulative cost of predecessors and self and
                   // heuristic

    Node() = default;

    UserState m_UserState;
  };

  // For sorting the heap the STL needs compare function that lets us compare
  // the f value of two nodes

  class HeapCompare {
  public:
    bool operator()(const Node *x, const Node *y) const { return x->f > y->f; }
  };

public: // methods
  // constructor just initialises private data
  AStarSearch()
      : state_(kSearchStateNotInitialised), current_solution_node_(nullptr),
#if USE_FSA_MEMORY
        m_FixedSizeAllocator(1000)

#endif
  {
  }

  explicit AStarSearch(int MaxNodes)
      : state_(kSearchStateNotInitialised), current_solution_node_(nullptr),
#if USE_FSA_MEMORY
        m_FixedSizeAllocator(MaxNodes)

#endif
  {
  }

  // call at any time to cancel the search and free up all the memory
  void CancelSearch() { m_CancelRequest = true; }

  // Set Start and goal states
  void SetStartAndGoalStates(UserState &Start, UserState &Goal) {
    start_ = AllocateNode();
    goal_ = AllocateNode();

    assert((start_ != nullptr && goal_ != nullptr));

    start_->m_UserState = Start;
    goal_->m_UserState = Goal;

    state_ = kSearchStateSearching;

    // Initialise the AStar specific parts of the Start Node
    // The user only needs fill out the state information

    start_->g = 0;
    start_->h = start_->m_UserState.GoalDistanceEstimate(goal_->m_UserState);
    start_->f = start_->g + start_->h;
    start_->parent = 0;

    // Push the start node on the Open list

    open_list_.push_back(start_); // heap now unsorted

    // Sort back element into heap
    push_heap(open_list_.begin(), open_list_.end(), HeapCompare());

    // Initialise counter for search steps
    steps_ = 0;
  }

  // Advances search one step
  unsigned int SearchStep() {
    // Firstly break if the user has not initialised the search
    assert((state_ > kSearchStateNotInitialised) &&
           (state_ < kSearchStateInvalid));

    // Next I want it to be safe to do a searchstep once the search has
    // succeeded...
    if ((state_ == kSearchStateSucceeded) || (state_ == kSearchStateFailed)) {
      return state_;
    }

    // Failure is defined as emptying the open list as there is nothing left
    // to search... New: Allow user abort
    if (open_list_.empty() || m_CancelRequest) {
      FreeAllNodes();
      state_ = kSearchStateFailed;
      return state_;
    }

    // Incremement step count
    steps_++;

    // Pop the best node (the one with the lowest f)
    Node *n = open_list_.front(); // get pointer to the node
    pop_heap(open_list_.begin(), open_list_.end(), HeapCompare());
    open_list_.pop_back();

    // Check for the goal, once we pop that we're done
    if (n->m_UserState.IsGoal(goal_->m_UserState)) {
      // The user is going to use the Goal Node he passed in
      // so copy the parent pointer of n
      goal_->parent = n->parent;
      goal_->g = n->g;

      // A special case is that the goal was passed in as the start state
      // so handle that here
      if (false == n->m_UserState.IsSameState(start_->m_UserState)) {
        FreeNode(n);

        // set the child pointers in each node (except Goal which has no
        // child)
        Node *nodeChild = goal_;
        Node *nodeParent = goal_->parent;

        do {
          nodeParent->child = nodeChild;

          nodeChild = nodeParent;
          nodeParent = nodeParent->parent;

        } while (nodeChild != start_); // Start is always the first
                                       // node by definition
      }

      // delete nodes that aren't needed for the solution
      FreeUnusedNodes();

      state_ = kSearchStateSucceeded;

      return state_;
    }
    // not goal
    // We now need to generate the successors of this node
    // The user helps us to do this, and we keep the new nodes in
    // m_Successors ...

    successors_.clear(); // empty vector of successor nodes to n

    // User provides this functions and uses AddSuccessor to add each
    // successor of node 'n' to m_Successors
    bool ret = n->m_UserState.GetSuccessors(
        this, n->parent ? &n->parent->m_UserState : nullptr);

    if (!ret) {
      typename std::vector<Node *>::iterator successor;

      // free the nodes that may previously have been added
      for (successor = successors_.begin(); successor != successors_.end();
           successor++) {
        FreeNode((*successor));
      }

      successors_.clear(); // empty vector of successor nodes to n

      // free up everything else we allocated
      FreeNode((n));
      FreeAllNodes();

      state_ = kSearchStateOutOfMemory;
      return state_;
    }

    // Now handle each successor to the current node ...
    for (typename std::vector<Node *>::iterator successor = successors_.begin();
         successor != successors_.end(); successor++) {
      // 	The g value for this successor ...
      float newg = n->g + n->m_UserState.GetCost((*successor)->m_UserState);

      // Now we need to find whether the node is on the open or closed
      // lists If it is but the node that is already on them is better
      // (lower g) then we can forget about this successor

      // First linear search of open list to find node

      typename std::vector<Node *>::iterator openlist_result;

      for (openlist_result = open_list_.begin();
           openlist_result != open_list_.end(); openlist_result++) {
        if ((*openlist_result)
                ->m_UserState.IsSameState((*successor)->m_UserState)) {
          break;
        }
      }

      if (openlist_result != open_list_.end()) {
        // we found this state on open

        if ((*openlist_result)->g <= newg) {
          FreeNode((*successor));

          // the one on Open is cheaper than this one
          continue;
        }
      }

      typename std::vector<Node *>::iterator closedlist_result;

      for (closedlist_result = closed_list_.begin();
           closedlist_result != closed_list_.end(); closedlist_result++) {
        if ((*closedlist_result)
                ->m_UserState.IsSameState((*successor)->m_UserState)) {
          break;
        }
      }

      if (closedlist_result != closed_list_.end()) {
        // we found this state on closed

        if ((*closedlist_result)->g <= newg) {
          // the one on Closed is cheaper than this one
          FreeNode((*successor));

          continue;
        }
      }

      // This node is the best node so far with this particular state
      // so lets keep it and set up its AStar specific data ...

      (*successor)->parent = n;
      (*successor)->g = newg;
      (*successor)->h =
          (*successor)->m_UserState.GoalDistanceEstimate(goal_->m_UserState);
      (*successor)->f = (*successor)->g + (*successor)->h;

      // Successor in closed list
      // 1 - Update old version of this node in closed list
      // 2 - Move it from closed to open list
      // 3 - Sort heap again in open list

      if (closedlist_result != closed_list_.end()) {
        // Update closed node with successor node AStar data
        //*(*closedlist_result) = *(*successor);
        (*closedlist_result)->parent = (*successor)->parent;
        (*closedlist_result)->g = (*successor)->g;
        (*closedlist_result)->h = (*successor)->h;
        (*closedlist_result)->f = (*successor)->f;

        // Free successor node
        FreeNode((*successor));

        // Push closed node into open list
        open_list_.push_back((*closedlist_result));

        // Remove closed node from closed list
        closed_list_.erase(closedlist_result);

        // Sort back element into heap
        push_heap(open_list_.begin(), open_list_.end(), HeapCompare());

        // Fix thanks to ...
        // Greg Douglas <gregdouglasmail@gmail.com>
        // who noticed that this code path was incorrect
        // Here we have found a new state which is already CLOSED
      }

      // Successor in open list
      // 1 - Update old version of this node in open list
      // 2 - sort heap again in open list

      else if (openlist_result != open_list_.end()) {
        // Update open node with successor node AStar data
        //*(*openlist_result) = *(*successor);
        (*openlist_result)->parent = (*successor)->parent;
        (*openlist_result)->g = (*successor)->g;
        (*openlist_result)->h = (*successor)->h;
        (*openlist_result)->f = (*successor)->f;

        // Free successor node
        FreeNode((*successor));

        // re-make the heap
        // make_heap rather than sort_heap is an essential bug fix
        // thanks to Mike Ryynanen for pointing this out and then
        // explaining it in detail. sort_heap called on an invalid
        // heap does not work
        make_heap(open_list_.begin(), open_list_.end(), HeapCompare());
      }

      // New successor
      // 1 - Move it from successors to open list
      // 2 - sort heap again in open list

      else {
        // Push successor node into open list
        open_list_.push_back((*successor));

        // Sort back element into heap
        push_heap(open_list_.begin(), open_list_.end(), HeapCompare());
      }
    }

    // push n onto Closed, as we have expanded it now

    closed_list_.push_back(n);

    return state_; // Succeeded bool is false at this point.
  }

  // User calls this to add a successor to a list of successors
  // when expanding the search frontier
  bool AddSuccessor(UserState &&State) {
    Node *node = AllocateNode();

    if (node) {
      node->m_UserState = std::move(State);

      successors_.push_back(node);

      return true;
    }

    return false;
  }

  // Free the solution nodes
  // This is done to clean up all used Node memory when you are done with the
  // search
  void FreeSolutionNodes() {
    Node *n = start_;

    if (start_->child) {
      do {
        Node *del = n;
        n = n->child;
        FreeNode(del);

        del = nullptr;

      } while (n != goal_);

      FreeNode(n); // Delete the goal
    } else {
      // if the start node is the solution we need to just delete the
      // start and goal nodes
      FreeNode(start_);
      FreeNode(goal_);
    }
  }

  // Functions for traversing the solution

  // Get start node
  UserState *GetSolutionStart() {
    current_solution_node_ = start_;
    if (start_) {
      return &start_->m_UserState;
    }
    return nullptr;
  }

  // Get next node
  UserState *GetSolutionNext() {
    if (current_solution_node_) {
      if (current_solution_node_->child) {
        Node *child = current_solution_node_->child;

        current_solution_node_ = current_solution_node_->child;

        return &child->m_UserState;
      }
    }

    return nullptr;
  }

  // Get end node
  UserState *GetSolutionEnd() {
    current_solution_node_ = goal_;
    if (goal_) {
      return &goal_->m_UserState;
    }
    return nullptr;
  }

  // Step solution iterator backwards
  UserState *GetSolutionPrev() {
    if (current_solution_node_) {
      if (current_solution_node_->parent) {
        Node *parent = current_solution_node_->parent;

        current_solution_node_ = current_solution_node_->parent;

        return &parent->m_UserState;
      }
    }

    return nullptr;
  }

  // Get final cost of solution
  // Returns FLT_MAX if goal is not defined or there is no solution
  float GetSolutionCost() {
    if (goal_ && state_ == kSearchStateSucceeded) {
      return goal_->g;
    }
    return FLT_MAX;
  }

  // For educational use and debugging it is useful to be able to view
  // the open and closed list at each step, here are two functions to allow
  // that.

  UserState *GetOpenListStart() {
    float f, g, h;
    return GetOpenListStart(f, g, h);
  }

  UserState *GetOpenListStart(float &f, float &g, float &h) {
    iterDbgOpen = open_list_.begin();
    if (iterDbgOpen != open_list_.end()) {
      f = (*iterDbgOpen)->f;
      g = (*iterDbgOpen)->g;
      h = (*iterDbgOpen)->h;
      return &(*iterDbgOpen)->m_UserState;
    }

    return nullptr;
  }

  UserState *GetOpenListNext() {
    float f, g, h;
    return GetOpenListNext(f, g, h);
  }

  UserState *GetOpenListNext(float &f, float &g, float &h) {
    iterDbgOpen++;
    if (iterDbgOpen != open_list_.end()) {
      f = (*iterDbgOpen)->f;
      g = (*iterDbgOpen)->g;
      h = (*iterDbgOpen)->h;
      return &(*iterDbgOpen)->m_UserState;
    }

    return nullptr;
  }

  UserState *GetClosedListStart() {
    float f, g, h;
    return GetClosedListStart(f, g, h);
  }

  UserState *GetClosedListStart(float &f, float &g, float &h) {
    iterDbgClosed = closed_list_.begin();
    if (iterDbgClosed != closed_list_.end()) {
      f = (*iterDbgClosed)->f;
      g = (*iterDbgClosed)->g;
      h = (*iterDbgClosed)->h;

      return &(*iterDbgClosed)->m_UserState;
    }

    return nullptr;
  }

  UserState *GetClosedListNext() {
    float f, g, h;
    return GetClosedListNext(f, g, h);
  }

  UserState *GetClosedListNext(float &f, float &g, float &h) {
    iterDbgClosed++;
    if (iterDbgClosed != closed_list_.end()) {
      f = (*iterDbgClosed)->f;
      g = (*iterDbgClosed)->g;
      h = (*iterDbgClosed)->h;

      return &(*iterDbgClosed)->m_UserState;
    }

    return nullptr;
  }

  // Get the number of steps

  int GetStepCount() { return steps_; }

  void EnsureMemoryFreed() {
#if USE_FSA_MEMORY
    assert(m_AllocateNodeCount == 0);
#endif
  }

private: // methods
  // This is called when a search fails or is cancelled to free all used
  // memory
  void FreeAllNodes() {
    // iterate open list and delete all nodes
    auto iter_open = open_list_.begin();
    while (iter_open != open_list_.end()) {
      Node *n = (*iter_open);
      FreeNode(n);
      iter_open++;
    }

    open_list_.clear();

    // iterate closed list and delete unused nodes

    for (auto iter_closed = closed_list_.begin();
         iter_closed != closed_list_.end(); iter_closed++) {
      Node *n = (*iter_closed);
      FreeNode(n);
    }

    closed_list_.clear();

    // delete the goal

    FreeNode(goal_);
  }

  // This call is made by the search class when the search ends. A lot of
  // nodes may be created that are still present when the search ends. They
  // will be deleted by this routine once the search ends
  void FreeUnusedNodes() {
    // iterate open list and delete unused nodes
    typename std::vector<Node *>::iterator iterOpen = open_list_.begin();

    while (iterOpen != open_list_.end()) {
      Node *n = (*iterOpen);

      if (!n->child) {
        FreeNode(n);

        n = nullptr;
      }

      iterOpen++;
    }

    open_list_.clear();

    // iterate closed list and delete unused nodes
    for (auto iter_closed = closed_list_.begin();
         iter_closed != closed_list_.end(); iter_closed++) {
      Node *n = (*iter_closed);

      if (!n->child) {
        FreeNode(n);
        n = nullptr;
      }
    }

    closed_list_.clear();
  }

  // Node memory management
  Node *AllocateNode() {
#if !USE_FSA_MEMORY
    m_AllocateNodeCount++;
    Node *p = new Node;
    return p;
#else
    Node *address = m_FixedSizeAllocator.alloc();

    if (!address) {
      return nullptr;
    }
    m_AllocateNodeCount++;
    Node *p = new (address) Node;
    return p;
#endif
  }

  void FreeNode(Node *node) {
    m_AllocateNodeCount--;

#if !USE_FSA_MEMORY
    delete node;
#else
    node->~Node();
    m_FixedSizeAllocator.free(node);
#endif
  }

  // Heap (simple vector but used as a heap, cf. Steve Rabin's game gems
  // article)
  std::vector<Node *> open_list_;

  // Closed list is a vector.
  std::vector<Node *> closed_list_;

  // Successors is a vector filled out by the user each type successors to a
  // node are generated
  std::vector<Node *> successors_;

  // State
  unsigned int state_;

  // Counts steps
  int steps_{};

  // Start and goal state pointers
  Node *start_;
  Node *goal_;

  Node *current_solution_node_;

#if USE_FSA_MEMORY
  // Memory
  FixedSizeAllocator<Node> m_FixedSizeAllocator;
#endif

  // Debug : need to keep these two iterators around
  // for the user Dbg functions
  typename std::vector<Node *>::iterator iterDbgOpen;
  typename std::vector<Node *>::iterator iterDbgClosed;

  // debugging : count memory allocation and free's
  int m_AllocateNodeCount = 0;

  bool m_CancelRequest = false;
};

template <class T> class AStarState {
public:
  virtual ~AStarState() = default;
  // Heuristic function which computes the estimated cost to the
  // goal node
  virtual float GoalDistanceEstimate(T &nodeGoal) = 0;
  // Returns true if this node is the goal node
  virtual bool IsGoal(T &nodeGoal) = 0;
  // Retrieves all successors to this node and adds them via
  // astarsearch.addSuccessor()
  virtual bool GetSuccessors(AStarSearch<T> *astarsearch, T *parent_node) = 0;
  // Computes the cost of travelling from this node to the successor
  // node
  virtual float GetCost(T &successor) = 0;
  // Returns true if this node is the same as the rhs node
  virtual bool IsSameState(T &rhs) = 0;
};
