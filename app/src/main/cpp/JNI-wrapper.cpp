#include <jni.h>
#include "fips205.h"
#include <string>
#include <vector>
#include <memory>

// Funciones de utilidad para conversiones JNI
ByteVector jbyteArrayToByteVector(JNIEnv* env, jbyteArray array) {
    if (array == nullptr) return ByteVector();

    jsize length = env->GetArrayLength(array);
    jbyte* bytes = env->GetByteArrayElements(array, nullptr);

    ByteVector result(bytes, bytes + length);

    env->ReleaseByteArrayElements(array, bytes, JNI_ABORT);
    return result;
}

jbyteArray byteVectorToJbyteArray(JNIEnv* env, const ByteVector& vec) {
    jbyteArray result = env->NewByteArray(static_cast<jsize>(vec.size()));
    if (result != nullptr) {
        env->SetByteArrayRegion(result, 0, static_cast<jsize>(vec.size()),
                                reinterpret_cast<const jbyte*>(vec.data()));
    }
    return result;
}

//Esta recibe el mensaje
void handleCppException(JNIEnv* env, std::exception_ptr e) {
    jclass exceptionClass = env->FindClass("java/lang/RuntimeException");

    try {
        if (e) {
            std::rethrow_exception(e);  // Relanza la excepción para capturarla
        }
    } catch (const std::exception& ex) {
        env->ThrowNew(exceptionClass, ex.what());  // Usa el mensaje de la excepción
        return;
    } catch (...) {
        env->ThrowNew(exceptionClass, "Unknown C++ exception");  // Para excepciones no estándar
        return;
    }
}

// Configuration Manager Functions

extern "C" JNIEXPORT jboolean JNICALL
Java_com_revelacion1_slh_1dsa_1library_SLHDSA_initializeConfig(
        JNIEnv* env, jobject /* this */,
        jint defaultScheme) {
    try {
        // Validar rango antes del cast
        if (defaultScheme < 0 || defaultScheme >= static_cast<jint>(SLH_DSA_ParamSet::PARAM_COUNT)) {
            return JNI_FALSE;
        }

        auto scheme = static_cast<SLH_DSA_ParamSet>(defaultScheme);

        // Realizar la inicialización
        FIPS205ConfigManager::initialize(scheme);

        // Verificar que la inicialización fue exitosa usando funciones existentes
        SLH_DSA_ParamSet currentScheme = FIPS205ConfigManager::getCurrentSchema();
        if (currentScheme != scheme) {
            return JNI_FALSE; // La inicialización no estableció el esquema correcto
        }

        // Verificar que los parámetros son válidos
        const SLH_DSA_Params* params = FIPS205ConfigManager::getCurrentParams();
        if (!params || params->n == 0) {
            return JNI_FALSE; // Parámetros inválidos
        }

        return JNI_TRUE;

    } catch(...) {
        handleCppException(env, std::current_exception());
        return JNI_FALSE;
    }
}

extern "C" JNIEXPORT jboolean JNICALL
Java_com_revelacion1_slh_1dsa_1library_SLHDSA_setParameterScheme(
        JNIEnv* env, jobject /* this */, jint scheme) {
    try {
        // Validar rango antes del cast
        if (scheme < 0 || scheme >= static_cast<jint>(SLH_DSA_ParamSet::PARAM_COUNT)) {
            return JNI_FALSE;
        }

        SLH_DSA_ParamSet paramSet = static_cast<SLH_DSA_ParamSet>(scheme);

        // Realizar el cambio de esquema
        bool success = FIPS205ConfigManager::setSchema(paramSet);
        if (!success) {
            return JNI_FALSE;
        }

        // Verificar que el cambio se aplicó correctamente usando funciones existentes
        SLH_DSA_ParamSet currentScheme = FIPS205ConfigManager::getCurrentSchema();
        if (currentScheme != paramSet) {
            return JNI_FALSE; // setSchema() devolvió true pero no se aplicó el cambio
        }

        // Verificar que los parámetros resultantes son válidos
        const SLH_DSA_Params* params = FIPS205ConfigManager::getCurrentParams();
        if (!params || params->n == 0 || params->h == 0) {
            return JNI_FALSE; // Parámetros resultantes inválidos
        }

        return JNI_TRUE;

    } catch(...) {
        handleCppException(env, std::current_exception());
        return JNI_FALSE;
    }
}

extern "C" JNIEXPORT jobjectArray JNICALL
Java_com_revelacion1_slh_1dsa_1library_SLHDSA_getCurrentParameters(
        JNIEnv* env, jobject /* this */) {
    try {
        const SLH_DSA_Params* params = FIPS205ConfigManager::getCurrentParams();
        if (!params) {
            return nullptr;
        }

        // Crear array de Object[3]
        jclass objectClass = env->FindClass("java/lang/Object");
        jobjectArray result = env->NewObjectArray(3, objectClass, nullptr);

        // [0] = String con el nombre
        jstring jname = env->NewStringUTF(params->name);
        env->SetObjectArrayElement(result, 0, jname);

        // [1] = IntArray con parámetros numéricos (EXTENDIDO a 11 para incluir sig_bytes)
        jintArray intParams = env->NewIntArray(11);  // ← CAMBIAR de 10 a 11
        jint values[11] = {  // ← CAMBIAR de 10 a 11
                static_cast<jint>(params->n),
                static_cast<jint>(params->h),
                static_cast<jint>(params->d),
                static_cast<jint>(params->h_prima),
                static_cast<jint>(params->a),
                static_cast<jint>(params->k),
                static_cast<jint>(params->lg_w),
                static_cast<jint>(params->m),
                static_cast<jint>(params->security_category),
                static_cast<jint>(params->pk_bytes),
                static_cast<jint>(params->sig_bytes)
        };
        env->SetIntArrayRegion(intParams, 0, 11, values);
        env->SetObjectArrayElement(result, 1, intParams);

        // [2] = Boolean para is_shake
        jclass booleanClass = env->FindClass("java/lang/Boolean");
        jmethodID booleanConstructor = env->GetMethodID(booleanClass, "<init>", "(Z)V");
        jobject jIsShake = env->NewObject(booleanClass, booleanConstructor,
                                          static_cast<jboolean>(params->is_shake));
        env->SetObjectArrayElement(result, 2, jIsShake);

        // Limpiar referencias locales
        env->DeleteLocalRef(jname);
        env->DeleteLocalRef(intParams);
        env->DeleteLocalRef(jIsShake);

        return result;

    } catch(...) {
        handleCppException(env, std::current_exception());
        return nullptr;
    }
}

extern "C" JNIEXPORT jstring JNICALL
Java_com_revelacion1_slh_1dsa_1library_SLHDSA_getCurrentSchemaName(
        JNIEnv* env, jobject /* this */) {
    try {
        const SLH_DSA_Params* params = FIPS205ConfigManager::getCurrentParams();
        if (!params || !params->name) {
            return env->NewStringUTF("Unknown");
        }
        return env->NewStringUTF(params->name);
    } catch(...) {
        handleCppException(env,std::current_exception());
        return env->NewStringUTF("Error");
    }
}

// ALGORITMOS SLH-DSA PRINCIPALES , preparados para ser llamados
extern "C" JNIEXPORT jobjectArray JNICALL
Java_com_revelacion1_slh_1dsa_1library_SLHDSA_slhKeyGen(
        JNIEnv* env, jobject /* this */) {
    try {
        auto keyPair = slh_keygen();

        jbyteArray publicKeyBytes = byteVectorToJbyteArray(env, keyPair.second.toBytes());
        jbyteArray privateKeyBytes = byteVectorToJbyteArray(env, keyPair.first.toBytes());

        jclass byteArrayClass = env->FindClass("[B");
        jobjectArray result = env->NewObjectArray(2, byteArrayClass, nullptr);

        env->SetObjectArrayElement(result, 0, publicKeyBytes);
        env->SetObjectArrayElement(result, 1, privateKeyBytes);

        return result;
    } catch (...) {
        handleCppException(env,std::current_exception());
        return nullptr;
    }
}

// Interfaces Externas
extern "C" JNIEXPORT jbyteArray JNICALL
Java_com_revelacion1_slh_1dsa_1library_SLHDSA_slhSign(
        JNIEnv* env, jobject /* this */,
        jbyteArray MBytes,
        jbyteArray ctxBytes,
        jbyteArray SKBytes) {  // ¡Sin paramSet!
    try {
        ByteVector M = jbyteArrayToByteVector(env, MBytes);
        ByteVector ctx = jbyteArrayToByteVector(env, ctxBytes);
        ByteVector SKData = jbyteArrayToByteVector(env, SKBytes);

        SLH_DSA_PrivateKey SK = SLH_DSA_PrivateKey::fromBytes(SKData);

        ByteVector signature = slh_sign(M, ctx, SK);
        return byteVectorToJbyteArray(env, signature);
    } catch (...) {
        handleCppException(env,std::current_exception());
        return nullptr;
    }
}
extern "C" JNIEXPORT jboolean JNICALL
Java_com_revelacion1_slh_1dsa_1library_SLHDSA_slhVerify(
        JNIEnv* env, jobject /* this */,
        jbyteArray MBytes,
        jbyteArray SIGBytes,
        jbyteArray ctxBytes,
        jbyteArray PKBytes) {
    try {
        ByteVector M = jbyteArrayToByteVector(env, MBytes);
        ByteVector SIG = jbyteArrayToByteVector(env, SIGBytes);
        ByteVector ctx = jbyteArrayToByteVector(env, ctxBytes);
        ByteVector PKData = jbyteArrayToByteVector(env, PKBytes);

        SLH_DSA_PublicKey PK = SLH_DSA_PublicKey::fromBytes(PKData);

        bool result = slh_verify(M, SIG, ctx, PK);
        return static_cast<jboolean>(result);
    } catch (...) {
        handleCppException(env,std::current_exception());
        return JNI_FALSE;
    }
}

