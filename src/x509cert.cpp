#include <node.h>
#include <v8.h>
#include <openssl/bio.h>
#include <openssl/x509.h>
#include <openssl/pem.h>
#include <openssl/rsa.h>
#include <openssl/evp.h>
#include <iostream>

using namespace v8;

class X509Cert : node::ObjectWrap{
    private:
        const char *data =
        "-----BEGIN CERTIFICATE-----\n"
        "-----END CERTIFICATE-----";
        BIO *bio;
        BIO *outbio;
        X509 *certificate;
        EVP_PKEY *pubkey;
        RSA *rsa;
    public:
        X509Cert() {}
        ~X509Cert() {}

        static v8::Persistent<FunctionTemplate> persistent_function_template;

        static void Init(Handle<Object> target){

            v8::HandleScope scope;
            v8::Local<FunctionTemplate> local_function_template = v8::FunctionTemplate::New(New);
            X509Cert::persistent_function_template = v8::Persistent<FunctionTemplate>::New(local_function_template);
            X509Cert::persistent_function_template->InstanceTemplate()->SetInternalFieldCount(1);
            X509Cert::persistent_function_template->SetClassName(v8::String::NewSymbol("Wrapper"));

            NODE_SET_PROTOTYPE_METHOD(X509Cert::persistent_function_template, "getPublicKey", GetPublicKey);

            target->Set(String::NewSymbol("wrapper"), X509Cert::persistent_function_template->GetFunction());
        }

        static Handle<Value> New(const Arguments& args){
            HandleScope scope;
            X509Cert* x509_instance = new X509Cert();
           // x509_instance->outbio = BIO_new(BIO_s_file());
          //  x509_instance->outbio = BIO_new_fp(stdout, BIO_NOCLOSE);
            x509_instance->outbio = BIO_new(BIO_s_mem());
            x509_instance->bio = BIO_new(BIO_s_mem());
            BIO_puts(x509_instance->bio, x509_instance->data);
            x509_instance->certificate = PEM_read_bio_X509(x509_instance->bio, NULL, NULL, NULL);
            x509_instance->Wrap(args.This());
            return args.This();
        }

        static Handle<Value> GetPublicKey(const Arguments& args){
            v8::HandleScope scope;
            X509Cert* x509_instance = node::ObjectWrap::Unwrap<X509Cert>(args.This());

            x509_instance->pubkey = X509_get_pubkey(x509_instance->certificate);
            PEM_write_bio_PUBKEY(x509_instance->outbio, x509_instance->pubkey);

            char* dt;
            long len = BIO_get_mem_data(x509_instance->outbio, &dt);
            std::string s(dt, len);

//            std::cout << s.c_str();

            BIO_set_close(x509_instance->outbio, BIO_NOCLOSE);
            BIO_free(x509_instance->outbio);

            return v8::String::New(s.c_str());
        }

};

v8::Persistent<FunctionTemplate> X509Cert::persistent_function_template;

extern "C" {
    static void init(Handle<Object> target){
        X509Cert::Init(target);
    }

    NODE_MODULE(x509cert, init);
}
