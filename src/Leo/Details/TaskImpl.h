#pragma once

#include "ResultImpl.h"

namespace Leo {
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
} // end of namespace Leo
