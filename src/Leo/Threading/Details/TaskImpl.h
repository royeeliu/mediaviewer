#pragma once

namespace Leo {
namespace Threading {
namespace Details {

template<typename T>
class TaskImpl;
template<>
class TaskImpl<void>;

template<typename ReturnTypeT>
class TaskImpl : public Referencable
{
};

template<>
class TaskImpl<void> : public Referencable
{
};

} // end of namespace Details
} // end of namespace Threading
} // end of namespace Leo
