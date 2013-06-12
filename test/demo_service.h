// generated from 'demo_service.rpc'

#pragma once

#ifndef RPC_SERVER_H_
#error please include server.h before including this file
#endif // RPC_SERVER_H_

#ifndef RPC_CLIENT_H_
#error please include client.h before including this file
#endif // RPC_CLIENT_H_

#include <errno.h>

namespace demo {

struct point3 {
    double x;
    double y;
    double z;
};

inline rpc::Marshal& operator <<(rpc::Marshal& m, const point3& o) {
    m << o.x;
    m << o.y;
    m << o.z;
    return m;
}

inline rpc::Marshal& operator >>(rpc::Marshal& m, point3& o) {
    m >> o.x;
    m >> o.y;
    m >> o.z;
    return m;
}

class MathService: public rpc::Service {
public:
    enum {
        FAST_PRIME = 0x1001,
        FAST_DOT_PROD = 0x1002,
        FAST_LARGE_STR_NOP = 0x1003,
        PRIME = 0x1004,
        DOT_PROD = 0x1005,
        LARGE_STR_NOP = 0x1006,
    };
    void reg_to(rpc::Server* svr) {
        svr->reg(FAST_PRIME, this, &MathService::__fast_prime__wrapper__);
        svr->reg(FAST_DOT_PROD, this, &MathService::__fast_dot_prod__wrapper__);
        svr->reg(FAST_LARGE_STR_NOP, this, &MathService::__fast_large_str_nop__wrapper__);
        svr->reg(PRIME, this, &MathService::__prime__wrapper__);
        svr->reg(DOT_PROD, this, &MathService::__dot_prod__wrapper__);
        svr->reg(LARGE_STR_NOP, this, &MathService::__large_str_nop__wrapper__);
    }
    // these RPC handler functions need to be implemented by user
    // for 'raw' handlers, remember to reply req, delete req, and sconn->release(); use sconn->run_async for heavy job
    virtual void fast_prime(const rpc::i32& n, rpc::i32* flag);
    virtual void fast_dot_prod(const point3& p1, const point3& p2, double* v);
    virtual void fast_large_str_nop(const std::string& str);
    virtual void prime(const rpc::i32& n, rpc::i32* flag);
    virtual void dot_prod(const point3& p1, const point3& p2, double* v);
    virtual void large_str_nop(const std::string& str);
private:
    void __fast_prime__wrapper__(rpc::Request* req, rpc::ServerConnection* sconn) {
        rpc::i32 in_0;
        req->m >> in_0;
        rpc::i32 out_0;
        this->fast_prime(in_0, &out_0);
        sconn->begin_reply(req);
        *sconn << out_0;
        sconn->end_reply();
        delete req;
        sconn->release();
    }
    void __fast_dot_prod__wrapper__(rpc::Request* req, rpc::ServerConnection* sconn) {
        point3 in_0;
        req->m >> in_0;
        point3 in_1;
        req->m >> in_1;
        double out_0;
        this->fast_dot_prod(in_0, in_1, &out_0);
        sconn->begin_reply(req);
        *sconn << out_0;
        sconn->end_reply();
        delete req;
        sconn->release();
    }
    void __fast_large_str_nop__wrapper__(rpc::Request* req, rpc::ServerConnection* sconn) {
        std::string in_0;
        req->m >> in_0;
        this->fast_large_str_nop(in_0);
        sconn->begin_reply(req);
        sconn->end_reply();
        delete req;
        sconn->release();
    }
    void __prime__wrapper__(rpc::Request* req, rpc::ServerConnection* sconn) {
        RUNNABLE_CLASS3(R, MathService*, thiz, rpc::Request*, req, rpc::ServerConnection*, sconn, {
            rpc::i32 in_0;
            req->m >> in_0;
            rpc::i32 out_0;
            thiz->prime(in_0, &out_0);
            sconn->begin_reply(req);
            *sconn << out_0;
            sconn->end_reply();
            delete req;
            sconn->release();
        });
        sconn->run_async(new R(this, req, sconn));
    }
    void __dot_prod__wrapper__(rpc::Request* req, rpc::ServerConnection* sconn) {
        RUNNABLE_CLASS3(R, MathService*, thiz, rpc::Request*, req, rpc::ServerConnection*, sconn, {
            point3 in_0;
            req->m >> in_0;
            point3 in_1;
            req->m >> in_1;
            double out_0;
            thiz->dot_prod(in_0, in_1, &out_0);
            sconn->begin_reply(req);
            *sconn << out_0;
            sconn->end_reply();
            delete req;
            sconn->release();
        });
        sconn->run_async(new R(this, req, sconn));
    }
    void __large_str_nop__wrapper__(rpc::Request* req, rpc::ServerConnection* sconn) {
        RUNNABLE_CLASS3(R, MathService*, thiz, rpc::Request*, req, rpc::ServerConnection*, sconn, {
            std::string in_0;
            req->m >> in_0;
            thiz->large_str_nop(in_0);
            sconn->begin_reply(req);
            sconn->end_reply();
            delete req;
            sconn->release();
        });
        sconn->run_async(new R(this, req, sconn));
    }
};

class MathProxy {
protected:
    rpc::Client* __cl__;
public:
    MathProxy(rpc::Client* cl): __cl__(cl) { }
    rpc::Future* async_fast_prime(const rpc::i32& n, const rpc::FutureAttr& __fu_attr__ = rpc::FutureAttr()) {
        rpc::Future* __fu__ = __cl__->begin_request(MathService::FAST_PRIME, __fu_attr__);
        if (__fu__ != NULL) {
            *__cl__ << n;
        }
        __cl__->end_request();
        return __fu__;
    }
    rpc::i32 fast_prime(const rpc::i32& n, rpc::i32* flag) {
        rpc::Future* __fu__ = this->async_fast_prime(n);
        if (__fu__ == NULL) {
            return ENOTCONN;
        }
        rpc::i32 __ret__ = __fu__->get_error_code();
        if (__ret__ == 0) {
            __fu__->get_reply() >> *flag;
        }
        __fu__->release();
        return __ret__;
    }
    rpc::Future* async_fast_dot_prod(const point3& p1, const point3& p2, const rpc::FutureAttr& __fu_attr__ = rpc::FutureAttr()) {
        rpc::Future* __fu__ = __cl__->begin_request(MathService::FAST_DOT_PROD, __fu_attr__);
        if (__fu__ != NULL) {
            *__cl__ << p1;
            *__cl__ << p2;
        }
        __cl__->end_request();
        return __fu__;
    }
    rpc::i32 fast_dot_prod(const point3& p1, const point3& p2, double* v) {
        rpc::Future* __fu__ = this->async_fast_dot_prod(p1, p2);
        if (__fu__ == NULL) {
            return ENOTCONN;
        }
        rpc::i32 __ret__ = __fu__->get_error_code();
        if (__ret__ == 0) {
            __fu__->get_reply() >> *v;
        }
        __fu__->release();
        return __ret__;
    }
    rpc::Future* async_fast_large_str_nop(const std::string& str, const rpc::FutureAttr& __fu_attr__ = rpc::FutureAttr()) {
        rpc::Future* __fu__ = __cl__->begin_request(MathService::FAST_LARGE_STR_NOP, __fu_attr__);
        if (__fu__ != NULL) {
            *__cl__ << str;
        }
        __cl__->end_request();
        return __fu__;
    }
    rpc::i32 fast_large_str_nop(const std::string& str) {
        rpc::Future* __fu__ = this->async_fast_large_str_nop(str);
        if (__fu__ == NULL) {
            return ENOTCONN;
        }
        rpc::i32 __ret__ = __fu__->get_error_code();
        __fu__->release();
        return __ret__;
    }
    rpc::Future* async_prime(const rpc::i32& n, const rpc::FutureAttr& __fu_attr__ = rpc::FutureAttr()) {
        rpc::Future* __fu__ = __cl__->begin_request(MathService::PRIME, __fu_attr__);
        if (__fu__ != NULL) {
            *__cl__ << n;
        }
        __cl__->end_request();
        return __fu__;
    }
    rpc::i32 prime(const rpc::i32& n, rpc::i32* flag) {
        rpc::Future* __fu__ = this->async_prime(n);
        if (__fu__ == NULL) {
            return ENOTCONN;
        }
        rpc::i32 __ret__ = __fu__->get_error_code();
        if (__ret__ == 0) {
            __fu__->get_reply() >> *flag;
        }
        __fu__->release();
        return __ret__;
    }
    rpc::Future* async_dot_prod(const point3& p1, const point3& p2, const rpc::FutureAttr& __fu_attr__ = rpc::FutureAttr()) {
        rpc::Future* __fu__ = __cl__->begin_request(MathService::DOT_PROD, __fu_attr__);
        if (__fu__ != NULL) {
            *__cl__ << p1;
            *__cl__ << p2;
        }
        __cl__->end_request();
        return __fu__;
    }
    rpc::i32 dot_prod(const point3& p1, const point3& p2, double* v) {
        rpc::Future* __fu__ = this->async_dot_prod(p1, p2);
        if (__fu__ == NULL) {
            return ENOTCONN;
        }
        rpc::i32 __ret__ = __fu__->get_error_code();
        if (__ret__ == 0) {
            __fu__->get_reply() >> *v;
        }
        __fu__->release();
        return __ret__;
    }
    rpc::Future* async_large_str_nop(const std::string& str, const rpc::FutureAttr& __fu_attr__ = rpc::FutureAttr()) {
        rpc::Future* __fu__ = __cl__->begin_request(MathService::LARGE_STR_NOP, __fu_attr__);
        if (__fu__ != NULL) {
            *__cl__ << str;
        }
        __cl__->end_request();
        return __fu__;
    }
    rpc::i32 large_str_nop(const std::string& str) {
        rpc::Future* __fu__ = this->async_large_str_nop(str);
        if (__fu__ == NULL) {
            return ENOTCONN;
        }
        rpc::i32 __ret__ = __fu__->get_error_code();
        __fu__->release();
        return __ret__;
    }
};

} // namespace demo


namespace demo {

inline void MathService::fast_dot_prod(const point3& p1, const point3& p2, double* v) {
    *v = p1.x * p2.x + p1.y * p2.y + p1.z * p2.z;
}

inline void MathService::fast_large_str_nop(const std::string& str) { }

inline void MathService::large_str_nop(const std::string& str) { }


}
