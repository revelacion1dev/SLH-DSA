# SLH-DSA Android Library

Una librería Android nativa para firmas digitales post-cuánticas basada en el estándar **SLH-DSA (Stateless Hash-Based Digital Signature Algorithm)** según **FIPS 205**.

## 🔐 Características

- **Implementación completa FIPS 205** con variantes SHAKE
- **6 esquemas de parámetros** soportados (128s/f, 192s/f, 256s/f)
- **Multiplataforma Android** (ARM32, ARM64, x86, x64)
- **API Kotlin/Java** fácil de usar
- **Resistente a computación cuántica**
- **Sin dependencias externas**

## 📦 Arquitecturas Soportadas

| Archivo AAR | Arquitectura | Tamaño | Descripción |
|-------------|--------------|--------|-------------|
| `SLHDSA-arm32-release.aar` | ARMv7 (32-bit) | 1.681 KB | Dispositivos ARM de 32 bits |
| `SLHDSA-arm64-release.aar` | ARM64 (64-bit) | 1.844 KB | Dispositivos ARM de 64 bits |
| `SLHDSA-x64-release.aar` | x86_64 | 1.989 KB | Emuladores y dispositivos x64 |
| `SLHDSA-x86-release.aar` | x86 (32-bit) | 1.921 KB | Emuladores x86 |
| `SLHDSA-mobile-release.aar` | ARM32 + ARM64 | 3.519 KB | Solo dispositivos móviles |
| `SLHDSA-universal-release.aar` | Todas | 7.416 KB | **Recomendado**: Todas las arquitecturas |

## 🚀 Instalación

### Opción 1: AAR Universal 
```gradle
// En tu app/build.gradle
dependencies {
    implementation files('libs/SLHDSA-universal-release.aar')
}
```

### Opción 2: Arquitectura Específica
```gradle
// Solo para dispositivos ARM64
dependencies {
    implementation files('libs/SLHDSA-arm64-release.aar')
}
```

### Configuración del Proyecto
1. Copia el archivo `.aar` correspondiente a la carpeta `app/libs/`
2. Añade la dependencia en `build.gradle`
3. Sincroniza el proyecto

## 📚 Uso Básico

### Inicialización
```kotlin
import com.revelacion1.slh_dsa_library.SLHDSA

val slhDsa = SLHDSA()

// Inicializar con esquema SLH-DSA-SHAKE-128s
if (slhDsa.initializeConfig(0)) {
    println("Librería inicializada correctamente")
} else {
    println("Error en inicialización")
}
```

### Generación de Claves
```kotlin
val keyPair = slhDsa.slhKeyGen()
if (keyPair != null) {
    val publicKey = keyPair[0]   // Clave pública
    val privateKey = keyPair[1]  // Clave privada
    
    println("Claves generadas - PK: ${publicKey.size} bytes, SK: ${privateKey.size} bytes")
}
```

### Firmar Mensaje
```kotlin
val mensaje = "Hola mundo post-cuántico!".toByteArray()
val contexto = byteArrayOf() // Contexto vacío

val firma = slhDsa.slhSign(mensaje, contexto, privateKey)
if (firma != null) {
    println("Mensaje firmado - Firma: ${firma.size} bytes")
}
```

### Verificar Firma
```kotlin
val esValida = slhDsa.slhVerify(mensaje, firma, contexto, publicKey)
if (esValida) {
    println("✅ Firma válida")
} else {
    println("❌ Firma inválida")
}
```

## 🔧 Esquemas de Parámetros

| ID | Esquema | Nivel Seguridad | Tamaño PK | Tamaño Firma | Velocidad |
|----|---------|-----------------|-----------|--------------|-----------|
| 0 | SLH-DSA-SHAKE-128s | 128 bits | 32 bytes | ~7KB | Lenta |
| 1 | SLH-DSA-SHAKE-128f | 128 bits | 32 bytes | ~17KB | **Rápida** |
| 2 | SLH-DSA-SHAKE-192s | 192 bits | 48 bytes | ~16KB | Lenta |
| 3 | SLH-DSA-SHAKE-192f | 192 bits | 48 bytes | ~35KB | **Rápida** |
| 4 | SLH-DSA-SHAKE-256s | 256 bits | 64 bytes | ~29KB | Lenta |
| 5 | SLH-DSA-SHAKE-256f | 256 bits | 64 bytes | ~49KB | **Rápida** |

### Cambiar Esquema
```kotlin
// Cambiar a SLH-DSA-SHAKE-256f (máxima seguridad, rápido)
if (slhDsa.setParameterScheme(5)) {
    val info = slhDsa.getCurrentSchemaInfo()
    println("Esquema actual: ${info?.name}")
    println("Nivel seguridad: ${info?.security_category}")
}
```

## 📋 Ejemplo Completo

```kotlin
class MainActivity : AppCompatActivity() {
    private lateinit var slhDsa: SLHDSA
    
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        
        // Inicializar SLH-DSA
        slhDsa = SLHDSA()
        
        if (!slhDsa.initializeConfig(1)) { // SLH-DSA-SHAKE-128f
            Log.e("SLH-DSA", "Error inicializando librería")
            return
        }
        
        // Generar claves
        val keyPair = slhDsa.slhKeyGen() ?: run {
            Log.e("SLH-DSA", "Error generando claves")
            return
        }
        
        val publicKey = keyPair[0]
        val privateKey = keyPair[1]
        
        // Firmar documento
        val documento = "Contrato importante".toByteArray()
        val contexto = "app_v1.0".toByteArray()
        
        val firma = slhDsa.slhSign(documento, contexto, privateKey) ?: run {
            Log.e("SLH-DSA", "Error firmando documento")
            return
        }
        
        // Verificar firma
        val esValida = slhDsa.slhVerify(documento, firma, contexto, publicKey)
        
        if (esValida) {
            Log.i("SLH-DSA", "✅ Documento auténtico")
        } else {
            Log.w("SLH-DSA", "❌ Documento comprometido")
        }
        
        // Mostrar información del esquema
        slhDsa.getCurrentSchemaInfo()?.let { info ->
            Log.i("SLH-DSA", """
                Esquema: ${info.name}
                Seguridad: ${info.security_category} bits
                Tamaño PK: ${info.pk_bytes} bytes
                Tamaño firma: ${info.sig_bytes} bytes
            """.trimIndent())
        }
    }
}
```

## ⚡ Consideraciones de Rendimiento

- **Variantes 'f' (fast)**: Firmas más rápidas pero mayor tamaño
- **Variantes 's' (small)**: Firmas más pequeñas pero más lentas
- **Recomendación**: Usar SHAKE-128f para la mayoría de aplicaciones

## 🛡️ Seguridad

- ✅ **Resistente a ataques cuánticos** (Shor, Grover)
- ✅ **Basado en funciones hash** (SHAKE256)
- ✅ **Estándar FIPS 205** oficial
- ⚠️ **Firmas grandes**: 7KB - 49KB según esquema
- ⚠️ **Una vez por clave**: No reutilizar claves privadas

## 🔗 Repositorio y Código Fuente

```bibtex
@misc{revelacion1dev_slh_dsa_2024,
  author       = {revelacion1dev},
  title        = {{SLH-DSA}: Stateless Hash-Based Digital Signature Algorithm Implementation},
  year         = {2025},
  publisher    = {GitHub},
  journal      = {GitHub repository},
  howpublished = {\url{https://github.com/revelacion1dev/SLH-DSA}},
  note         = {Código fuente disponible en: \url{https://github.com/revelacion1dev/SLH-DSA}},
  url          = {https://github.com/revelacion1dev/SLH-DSA}
}
```


