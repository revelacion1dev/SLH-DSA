package com.revelacion1.slh_dsa_library

import android.util.Log

/**
 * Librería SLH-DSA para firmas digitales post-cuánticas
 * Implementación FIPS 205 con variantes SHAKE únicamente
 */
class SLHDSA {

    companion object {
        init {
            System.loadLibrary("slh_dsa_library")
        }
    }

    // CONFIGURATION MANAGER

    /**
     * Inicializa la librería con un esquema específico
     * @param defaultScheme Esquema a usar (0-5):
     *   0: SLH-DSA-SHAKE-128s, 1: SLH-DSA-SHAKE-128f
     *   2: SLH-DSA-SHAKE-192s, 3: SLH-DSA-SHAKE-192f
     *   4: SLH-DSA-SHAKE-256s, 5: SLH-DSA-SHAKE-256f
     * @return true si inicialización exitosa, false si error
     */
    external fun initializeConfig(defaultScheme: Int): Boolean

    /**
     * Cambia el esquema de parámetros actual
     * @param scheme Nuevo esquema (0-5, mismos valores que initializeConfig)
     * @return true si cambio exitoso, false si error asd
     */
    external fun setParameterScheme(scheme: Int): Boolean

    /**
     * Obtiene parámetros del esquema actual
     * @return Array[3]: [0]=String nombre, [1]=IntArray parámetros, [2]=Boolean isShake
     */
    external fun getCurrentParameters(): Array<Any>

    /**
     * Obtiene nombre del esquema actual
     * @return String con nombre del esquema o "Unknown"
     */
    external fun getCurrentSchemaName(): String

    // OPERACIONES CRIPTOGRÁFICAS

    /**
     * Genera par de claves SLH-DSA
     * @return Array[2]: [0]=clave pública, [1]=clave privada, null si error
     */
    external fun slhKeyGen(): Array<ByteArray>

    /**
     * Firma un mensaje con SLH-DSA
     * @param M Mensaje a firmar
     * @param ctx Contexto (separación de dominios, puede ser vacío)
     * @param SK Clave privada
     * @return Firma digital o null si error
     */
    external fun slhSign(M: ByteArray, ctx: ByteArray, SK: ByteArray): ByteArray

    /**
     * Verifica firma SLH-DSA
     * @param M Mensaje original
     * @param SIG Firma a verificar
     * @param ctx Contexto usado en firmado
     * @param PK Clave pública
     * @return true si firma válida, false si inválida
     */
    external fun slhVerify(M: ByteArray, SIG: ByteArray, ctx: ByteArray, PK: ByteArray): Boolean

    // FUNCIÓN AUXILIAR

    /**
     * Obtiene información estructurada del esquema actual
     * @return SchemaInfo con parámetros del esquema o null si error
     */
    fun getCurrentSchemaInfo(): SchemaInfo? {
        return try {
            val array = getCurrentParameters()

            val name = array[0] as String
            val intParams = array[1] as IntArray
            val isShake = array[2] as Boolean

            SchemaInfo(
                name = name,
                n = intParams[0],
                h = intParams[1],
                d = intParams[2],
                h_prima = intParams[3],
                a = intParams[4],
                k = intParams[5],
                lg_w = intParams[6],
                m = intParams[7],
                security_category = intParams[8],
                pk_bytes = intParams[9],
                sig_bytes = intParams[10],
                is_shake = isShake
            )
        } catch (e: Exception) {
            Log.e("SLHDSA", "Error parsing schema info: ${e.message}")
            null
        }
    }

    /**
     * Información del esquema SLH-DSA actual
     * @property name Nombre del esquema
     * @property n Parámetro de seguridad
     * @property h Altura total del árbol
     * @property d Número de capas
     * @property h_prima Altura de subárbol
     * @property a Número de árboles FORS
     * @property k Hojas por árbol FORS
     * @property lg_w Log2 parámetro Winternitz
     * @property m Longitud mensaje interno
     * @property security_category Nivel seguridad (1, 3, 5)
     * @property pk_bytes Tamaño clave pública
     * @property sig_bytes Tamaño firma
     * @property is_shake Usa SHAKE (siempre true)
     */
    data class SchemaInfo(
        val name: String,
        val n: Int,
        val h: Int,
        val d: Int,
        val h_prima: Int,
        val a: Int,
        val k: Int,
        val lg_w: Int,
        val m: Int,
        val security_category: Int,
        val pk_bytes: Int,
        val sig_bytes: Int,
        val is_shake: Boolean
    )
}