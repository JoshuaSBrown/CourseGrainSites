
#include "mythical/queue.hpp"

#include <algorithm>
#include <iostream>

using namespace std;

namespace {
  struct Comparator {
    explicit Comparator(pair<int,double> value) : value_(value) { }
    inline bool operator()(const pair<int,double> & value) const {
      return value_.second < value.second;
    }
    private:
    pair<int,double> value_;
  };

  struct less_than
  {
    inline bool operator() (const pair<int,double>& struct1, const pair<int,double>& struct2)
    {
      return (struct1.second < struct2.second);
    }
  }; 
}

namespace mythical {

  size_t Queue::size() const noexcept { return walker_queue_.size(); }

  void Queue::add(pair<int,double> walker){
    walker_queue_.push_back(walker);
    sorted_ = false;
  }

  void Queue::sortedAdd(pair<int,double> walker){
    Comparator comp(walker);
    auto it = find_if(walker_queue_.begin(),walker_queue_.end(),comp);
    walker_queue_.insert(it,walker);
  }

  pair<int,double> Queue::pop_current() {
    auto current = walker_queue_.front();
    walker_queue_.pop_front();
    return current;
  }

  bool Queue::isSorted() const noexcept { return sorted_ ; }

  void Queue::sort() {
    if ( sorted_ ) return;
    std::sort( walker_queue_.begin(), walker_queue_.end(), less_than() );
    sorted_ = true;
  }

  const pair<int,double> & Queue::at(int index) const {
    return walker_queue_.at(index); 
  }
  
}
