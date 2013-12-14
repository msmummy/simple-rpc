#include <Python.h>

#include <string>

#include "rpc/server.h"
#include "rpc/client.h"

using namespace rpc;

class GILHelper {
    PyGILState_STATE gil_state;
public:
    GILHelper() {
        gil_state = PyGILState_Ensure();
    }

    ~GILHelper() {
        PyGILState_Release(gil_state);
    }
};

static PyObject* _pyrpc_init_server(PyObject* self, PyObject* args) {
    GILHelper gil_helper;
//    Log::info("init_server called");
    Server* svr = new Server;
    return Py_BuildValue("k", svr);
}

static PyObject* _pyrpc_fini_server(PyObject* self, PyObject* args) {
    GILHelper gil_helper;
    // Log::info("fini_server called");
    unsigned long u;
    if (!PyArg_ParseTuple(args, "k", &u))
        return nullptr;
    Server* svr = (Server *) u;
    delete svr;
    Py_RETURN_NONE;
}

static PyObject* _pyrpc_server_start(PyObject* self, PyObject* args) {
    GILHelper gil_helper;
    // Log::info("server_start called");
    const char* addr;
    unsigned long u;
    if (!PyArg_ParseTuple(args, "ks", &u, &addr))
        return nullptr;
    Server* svr = (Server *) u;
    int ret = svr->start(addr);
    return Py_BuildValue("i", ret);
}

static PyObject* _pyrpc_server_unreg(PyObject* self, PyObject* args) {
    GILHelper gil_helper;
    // Log::info("server_unreg called");
    unsigned long u;
    int rpc_id;
    if (!PyArg_ParseTuple(args, "ki", &u, &rpc_id))
        return nullptr;
    Server* svr = (Server *) u;
    svr->unreg(rpc_id);
    Py_RETURN_NONE;
}

static PyObject* _pyrpc_server_reg(PyObject* self, PyObject* args) {
    GILHelper gil_helper;
    unsigned long u;
    int rpc_id;
    PyObject* func;
    if (!PyArg_ParseTuple(args, "kiO", &u, &rpc_id, &func))
        return nullptr;
    Server* svr = (Server *) u;

    // incr ref_count on PyObject func
    // TODO remember this, and decr ref_count on PyObject func when shutting down server
    Py_XINCREF(func);

    int ret = svr->reg(rpc_id, [func](Request* req, ServerConnection* sconn) {
        Marshal* output_m = nullptr;
        {
            unsigned long u = (unsigned long) &req->m;
            GILHelper gil_helper;
            PyObject* params = Py_BuildValue("(k)", u);
            PyObject* result = PyObject_CallObject(func, params);
            output_m = (Marshal *) PyInt_AsLong(result);
            Py_XDECREF(params);
            Py_XDECREF(result);
        }

        sconn->begin_reply(req);
        if (output_m != nullptr) {
            *sconn << *output_m;
        }
        sconn->end_reply();

        if (output_m != nullptr) {
            delete output_m;
        }

        // cleanup as required by simple-rpc
        delete req;
        sconn->release();
    });

    return Py_BuildValue("i", ret);
}

static PyObject* _pyrpc_init_poll_mgr(PyObject* self, PyObject* args) {
    GILHelper gil_helper;
    // Log::info("init_poll_mgr called");
    PollMgr* poll = new PollMgr;
    return Py_BuildValue("k", poll);
}

static PyObject* _pyrpc_init_client(PyObject* self, PyObject* args) {
    GILHelper gil_helper;
    // Log::info("init_client called");
    unsigned long u;
    if (!PyArg_ParseTuple(args, "k", &u))
        return nullptr;
    PollMgr* poll = (PollMgr *) u;
    Client* clnt = new Client(poll);
    return Py_BuildValue("k", clnt);
}

static PyObject* _pyrpc_fini_client(PyObject* self, PyObject* args) {
    GILHelper gil_helper;
    // Log::info("fini_client called");
    unsigned long u;
    if (!PyArg_ParseTuple(args, "k", &u))
        return nullptr;
    Client* clnt = (Client *) u;
    clnt->close_and_release();
    Py_RETURN_NONE;
}

static PyObject* _pyrpc_client_connect(PyObject* self, PyObject* args) {
    GILHelper gil_helper;
    // Log::info("client_connect called");
    const char* addr;
    unsigned long u;
    if (!PyArg_ParseTuple(args, "ks", &u, &addr))
        return nullptr;
    Client* clnt = (Client *) u;
    int ret = clnt->connect(addr);
    return Py_BuildValue("i", ret);
}

static PyObject* _pyrpc_client_sync_call(PyObject* self, PyObject* args) {
    GILHelper gil_helper;
    // Log::info("client_sync_call called");

    PyThreadState *_save;
    _save = PyEval_SaveThread();

    unsigned long u;
    int rpc_id;
    const char* enc_args;
    if (!PyArg_ParseTuple(args, "kis", &u, &rpc_id, &enc_args))
        return nullptr;

    Client* clnt = (Client *) u;

    Future* fu = clnt->begin_request(rpc_id);
    if (fu != nullptr) {
        *clnt << std::string(enc_args);
    }
    clnt->end_request();

    std::string enc_result;
    int error_code;
    if (fu == nullptr) {
        error_code = ENOTCONN;
    } else {
        error_code = fu->get_error_code();
        if (error_code == 0) {
            fu->get_reply() >> enc_result;
        }
        fu->release();
    }

    PyEval_RestoreThread(_save);

    return Py_BuildValue("(is)", error_code, enc_result.c_str());
}


static PyObject* _pyrpc_init_marshal(PyObject* self, PyObject* args) {
    GILHelper gil_helper;
    Marshal* m = new Marshal;
    return Py_BuildValue("k", m);
}

static PyObject* _pyrpc_fini_marshal(PyObject* self, PyObject* args) {
    GILHelper gil_helper;
    unsigned long u;
    if (!PyArg_ParseTuple(args, "k", &u))
        return nullptr;
    Marshal* m = (Marshal *) u;
    delete m;
    Py_RETURN_NONE;
}


static PyObject* _pyrpc_marshal_size(PyObject* self, PyObject* args) {
    GILHelper gil_helper;
    unsigned long u;
    if (!PyArg_ParseTuple(args, "k", &u))
        return nullptr;
    Marshal* m = (Marshal *) u;
    return Py_BuildValue("k", m->content_size());
}

static PyObject* _pyrpc_marshal_write_i32(PyObject* self, PyObject* args) {
    GILHelper gil_helper;
    unsigned long u;
    long vl;
    if (!PyArg_ParseTuple(args, "kl", &u, &vl))
        return nullptr;
    Marshal* m = (Marshal *) u;
    rpc::i32 v = (rpc::i32) vl;
    *m << v;
    Py_RETURN_NONE;
}


static PyObject* _pyrpc_marshal_read_i32(PyObject* self, PyObject* args) {
    GILHelper gil_helper;
    unsigned long u;
    if (!PyArg_ParseTuple(args, "k", &u))
        return nullptr;
    Marshal* m = (Marshal *) u;
    rpc::i32 v;
    *m >> v;
    long vl = v;
    return Py_BuildValue("l", vl);
}

static PyObject* _pyrpc_marshal_write_i64(PyObject* self, PyObject* args) {
    GILHelper gil_helper;
    unsigned long u;
    long long vll;
    if (!PyArg_ParseTuple(args, "kL", &u, &vll))
        return nullptr;
    Marshal* m = (Marshal *) u;
    rpc::i64 v = (rpc::i64) vll;
    *m << v;
    Py_RETURN_NONE;
}


static PyObject* _pyrpc_marshal_read_i64(PyObject* self, PyObject* args) {
    GILHelper gil_helper;
    unsigned long u;
    if (!PyArg_ParseTuple(args, "k", &u))
        return nullptr;
    Marshal* m = (Marshal *) u;
    rpc::i64 v;
    *m >> v;
    long long vll = v;
    return Py_BuildValue("L", vll);
}


static PyObject* _pyrpc_marshal_write_double(PyObject* self, PyObject* args) {
    GILHelper gil_helper;
    unsigned long u;
    double dbl;
    if (!PyArg_ParseTuple(args, "kd", &u, &dbl))
        return nullptr;
    Marshal* m = (Marshal *) u;
    *m << dbl;
    Py_RETURN_NONE;
}


static PyObject* _pyrpc_marshal_read_double(PyObject* self, PyObject* args) {
    GILHelper gil_helper;
    unsigned long u;
    if (!PyArg_ParseTuple(args, "k", &u))
        return nullptr;
    Marshal* m = (Marshal *) u;
    double dbl;
    *m >> dbl;
    return Py_BuildValue("d", dbl);
}



static PyObject* _pyrpc_marshal_write_str(PyObject* self, PyObject* args) {
    GILHelper gil_helper;
    unsigned long u;
    PyObject* str_obj;
    if (!PyArg_ParseTuple(args, "kO", &u, &str_obj))
        return nullptr;
    Marshal* m = (Marshal *) u;
    std::string str(PyString_AsString(str_obj), PyString_Size(str_obj));
    *m << str;
    Py_RETURN_NONE;
}


static PyObject* _pyrpc_marshal_read_str(PyObject* self, PyObject* args) {
    GILHelper gil_helper;
    unsigned long u;
    if (!PyArg_ParseTuple(args, "k", &u))
        return nullptr;
    Marshal* m = (Marshal *) u;
    std::string str;
    *m >> str;
    PyObject* str_obj = PyString_FromStringAndSize(&str[0], str.size());
    return Py_BuildValue("O", str_obj);
}


static PyMethodDef _pyrpcMethods[] = {
    {"init_server", _pyrpc_init_server, METH_VARARGS, nullptr},
    {"fini_server", _pyrpc_fini_server, METH_VARARGS, nullptr},
    {"server_start", _pyrpc_server_start, METH_VARARGS, nullptr},
    {"server_unreg", _pyrpc_server_unreg, METH_VARARGS, nullptr},
    {"server_reg", _pyrpc_server_reg, METH_VARARGS, nullptr},

    {"init_poll_mgr", _pyrpc_init_poll_mgr, METH_VARARGS, nullptr},

    {"init_client", _pyrpc_init_client, METH_VARARGS, nullptr},
    {"fini_client", _pyrpc_fini_client, METH_VARARGS, nullptr},
    {"client_connect", _pyrpc_client_connect, METH_VARARGS, nullptr},
    {"client_sync_call", _pyrpc_client_sync_call, METH_VARARGS, nullptr},

    {"init_marshal", _pyrpc_init_marshal, METH_VARARGS, nullptr},
    {"fini_marshal", _pyrpc_fini_marshal, METH_VARARGS, nullptr},
    {"marshal_size", _pyrpc_marshal_size, METH_VARARGS, nullptr},
    {"marshal_write_i32", _pyrpc_marshal_write_i32, METH_VARARGS, nullptr},
    {"marshal_read_i32", _pyrpc_marshal_read_i32, METH_VARARGS, nullptr},
    {"marshal_write_i64", _pyrpc_marshal_write_i64, METH_VARARGS, nullptr},
    {"marshal_read_i64", _pyrpc_marshal_read_i64, METH_VARARGS, nullptr},
    {"marshal_write_double", _pyrpc_marshal_write_double, METH_VARARGS, nullptr},
    {"marshal_read_double", _pyrpc_marshal_read_double, METH_VARARGS, nullptr},
    {"marshal_write_str", _pyrpc_marshal_write_str, METH_VARARGS, nullptr},
    {"marshal_read_str", _pyrpc_marshal_read_str, METH_VARARGS, nullptr},

    {nullptr, nullptr, 0, nullptr}
};

PyMODINIT_FUNC init_pyrpc(void) {
    PyEval_InitThreads();
    // Log::debug("PyEval_InitThreads called!");
    GILHelper gil_helper;
    PyObject* m;
    m = Py_InitModule("_pyrpc", _pyrpcMethods);
    if (m == nullptr)
        return;
}
