#pragma once

#include "graph/root.hpp"

namespace Graph {


class AsyncRoot : public Root
{
public:
    AsyncRoot();

    /*
     *  Lots of overrides here, which mostly call Root::FUNCTION and then log
     *  a bunch of data to the changelog queue.
     */
    void insertCell(const SheetIndex& sheet, const CellIndex& cell,
                    const std::string& name, const std::string& expr) override;
    void insertInstance(const SheetIndex& parent, const InstanceIndex& i,
                        const std::string& name, const SheetIndex& target) override;
    void eraseCell(const CellIndex& cell) override;
    void eraseInstance(const InstanceIndex& instance) override;
    bool setExpr(const CellIndex& c, const std::string& expr) override;
    bool setInput(const InstanceIndex& instance, const CellIndex& cell,
                  const std::string& expr) override;
    bool renameItem(const ItemIndex& i, const std::string& name) override;
    void insertSheet(const SheetIndex& parent, const SheetIndex& sheet,
                     const std::string& name) override;
    void renameSheet(const SheetIndex& sheet, const std::string& name) override;
    void eraseSheet(const SheetIndex& s) override;
    void gotResult(const CellKey& k, const Value& result) override;
    void clearDeps(const CellKey& k) override;

    /*
     *  Start an async evaluation thread running.
     *
     *  The thread takes commands from input and puts responses out on the
     *  returned queue.
     *
     *  This moves the graph into the async thread; all operations must
     *  be done by injecting commands into the input queue after this
     *  point.
     */
    shared_queue<Response>& run(shared_queue<Command>& input);

    /*
     *  Waits for run to complete
     *
     *  The async master should halt the queue before
     *  calling this, otherwise the _run loop won't stop.
     */
    void wait();

    /*
     *  Installs a translator, which escapes values from the graph thread
     *  into some other thread.
     */
    void installTranslator(Translator* t) { translator = t; }

    /*
     *  Pushes a SERIALIZED response down the pipe
     */
    void serialize();

    /*
     *  Deserializes a string, pushing a DESERIALIZED Response
     */
    std::string loadString(const std::string& json) override;

protected:
    /*
     *  Async run function
     *  run() calls this in another thread!
     */
    void _run(shared_queue<Command>& input);

    /*
     *  Pushes a change, unless we're in a nested callSheet evaluation
     */
    void pushChange(const Response& response);

    /*  Queue of changes  */
    shared_queue<Response> changes;

    /*  The translator allows the graph to translate ValuePtr objects
     *  into types that can safely be moved out of the interpreter thread */
    Translator* translator=nullptr;

    /*  Future for async running  */
    std::future<void> future;
};

}   // namespace Graph
