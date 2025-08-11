#include "GraphicsEngine.pch.h"
#include "GraphicsCommandList.h"
#include "GraphicsEngine.h"
#include <stdexcept>

GraphicsCommandListIterator::GraphicsCommandListIterator(const GraphicsCommandList* aCommandList)
{
    myPtr = aCommandList->myRoot;
}

GraphicsCommandBase* GraphicsCommandListIterator::Next()
{
    GraphicsCommandBase* cmd = myPtr;
    myPtr = cmd->Next;
    return cmd;
}

GraphicsCommandList::GraphicsCommandList(size_t aMemorySize) : mySize(aMemorySize), 
                                                               myData(new uint8_t[mySize]), 
                                                               myRoot(reinterpret_cast<GraphicsCommandBase*>(myData)),
                                                               myLink(&myRoot)
{
    memset(myData, 0, mySize);
}

GraphicsCommandList::~GraphicsCommandList()
{
    Reset();

    delete[] myData;
    myData = nullptr;
    myRoot = nullptr;
    myLink = nullptr;
}

void GraphicsCommandList::Execute()
{
    if (!isExecuting && !isFinished)
    {
        isExecuting = true;
        GraphicsCommandListIterator it(this);

        while (it)
        {
            GraphicsCommandBase* cmd = it.Next();
            cmd->Execute();
            cmd->~GraphicsCommandBase();
        }

        isFinished = true;
        isExecuting = false;
    }
}

void GraphicsCommandList::Reset()
{
    if (isExecuting)
    {
        throw std::runtime_error("Can not reset a command list while it is executing");
    }

    if (myNumCommands > 0)
    {
        if (!isFinished)
        {
            GraphicsCommandListIterator it(this);

            while (it)
            {
                GraphicsCommandBase* cmd = it.Next();
                cmd->~GraphicsCommandBase();
            }
        }

        myRoot = reinterpret_cast<GraphicsCommandBase*>(myData);
        myLink = &myRoot;
        myCursor = 0;
        myNumCommands = 0;
    }

    isExecuting = false;
    isFinished = false;
}
