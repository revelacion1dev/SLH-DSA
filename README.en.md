# SLH-DSA Android Library

A native Android library for post-quantum digital signatures based on the **SLH-DSA (Stateless Hash-Based Digital Signature Algorithm)** standard according to **FIPS 205**.

## 🔐 Features

- **Complete FIPS 205 implementation** with SHAKE variants
- **6 parameter schemes** supported (128s/f, 192s/f, 256s/f)
- **Cross-platform Android** (ARM32, ARM64, x86, x64)
- **Easy-to-use Kotlin/Java API**
- **Quantum-resistant**
- **No external dependencies**

## 📦 Supported Architectures

| AAR File | Architecture | Size | Description |
|----------|--------------|------|-------------|
| `SLHDSA-arm32-release.aar` | ARMv7 (32-bit) | 1.681 KB | 32-bit ARM devices |
| `SLHDSA-arm64-release.aar` | ARM64 (64-bit) | 1.844 KB | 64-bit ARM devices |
| `SLHDSA-x64-release.aar` | x86_64 | 1.989 KB | x64 emulators and devices |
| `SLHDSA-x86-release.aar` | x86 (32-bit) | 1.921 KB | x86 emulators |
| `SLHDSA-mobile-release.aar` | ARM32 + ARM64 | 3.519 KB | Mobile devices only |
| `SLHDSA-universal-release.aar` | All | 7.416 KB | **Recommended**: All architectures |

## 🚀 Installation

### Option 1: Universal AAR 
```gradle
// In your app/build.gradle
dependencies {
    implementation files('libs/SLHDSA-universal-release.aar')
}
```

### Option 2: Specific Architecture
```gradle
// Only for ARM64 devices
dependencies {
    implementation files('libs/SLHDSA-arm64-release.aar')
}
```

### Project Configuration
1. Copy the corresponding `.aar` file to the `app/libs/` folder
2. Add the dependency in `build.gradle`
3. Sync the project

## 📚 Basic Usage

### Initialization
```kotlin
import com.revelacion1.slh_dsa_library.SLHDSA

val slhDsa = SLHDSA()

// Initialize with SLH-DSA-SHAKE-128s scheme
if (slhDsa.initializeConfig(0)) {
    println("Library initialized successfully")
} else {
    println("Initialization error")
}
```

### Key Generation
```kotlin
val keyPair = slhDsa.slhKeyGen()
if (keyPair != null) {
    val publicKey = keyPair[0]   // Public key
    val privateKey = keyPair[1]  // Private key
    
    println("Keys generated - PK: ${publicKey.size} bytes, SK: ${privateKey.size} bytes")
}
```

### Sign Message
```kotlin
val message = "Hello post-quantum world!".toByteArray()
val context = byteArrayOf() // Empty context

val signature = slhDsa.slhSign(message, context, privateKey)
if (signature != null) {
    println("Message signed - Signature: ${signature.size} bytes")
}
```

### Verify Signature
```kotlin
val isValid = slhDsa.slhVerify(message, signature, context, publicKey)
if (isValid) {
    println("✅ Valid signature")
} else {
    println("❌ Invalid signature")
}
```

## 🔧 Parameter Schemes

| ID | Scheme | Security Level | PK Size | Signature Size | Speed |
|----|--------|----------------|---------|----------------|-------|
| 0 | SLH-DSA-SHAKE-128s | 128 bits | 32 bytes | ~7KB | Slow |
| 1 | SLH-DSA-SHAKE-128f | 128 bits | 32 bytes | ~17KB | **Fast** |
| 2 | SLH-DSA-SHAKE-192s | 192 bits | 48 bytes | ~16KB | Slow |
| 3 | SLH-DSA-SHAKE-192f | 192 bits | 48 bytes | ~35KB | **Fast** |
| 4 | SLH-DSA-SHAKE-256s | 256 bits | 64 bytes | ~29KB | Slow |
| 5 | SLH-DSA-SHAKE-256f | 256 bits | 64 bytes | ~49KB | **Fast** |

### Change Scheme
```kotlin
// Change to SLH-DSA-SHAKE-256f (maximum security, fast)
if (slhDsa.setParameterScheme(5)) {
    val info = slhDsa.getCurrentSchemaInfo()
    println("Current scheme: ${info?.name}")
    println("Security level: ${info?.security_category}")
}
```

## 📋 Complete Example

```kotlin
class MainActivity : AppCompatActivity() {
    private lateinit var slhDsa: SLHDSA
    
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        
        // Initialize SLH-DSA
        slhDsa = SLHDSA()
        
        if (!slhDsa.initializeConfig(1)) { // SLH-DSA-SHAKE-128f
            Log.e("SLH-DSA", "Error initializing library")
            return
        }
        
        // Generate keys
        val keyPair = slhDsa.slhKeyGen() ?: run {
            Log.e("SLH-DSA", "Error generating keys")
            return
        }
        
        val publicKey = keyPair[0]
        val privateKey = keyPair[1]
        
        // Sign document
        val document = "Important contract".toByteArray()
        val context = "app_v1.0".toByteArray()
        
        val signature = slhDsa.slhSign(document, context, privateKey) ?: run {
            Log.e("SLH-DSA", "Error signing document")
            return
        }
        
        // Verify signature
        val isValid = slhDsa.slhVerify(document, signature, context, publicKey)
        
        if (isValid) {
            Log.i("SLH-DSA", "✅ Authentic document")
        } else {
            Log.w("SLH-DSA", "❌ Compromised document")
        }
        
        // Show scheme information
        slhDsa.getCurrentSchemaInfo()?.let { info ->
            Log.i("SLH-DSA", """
                Scheme: ${info.name}
                Security: ${info.security_category} bits
                PK size: ${info.pk_bytes} bytes
                Signature size: ${info.sig_bytes} bytes
            """.trimIndent())
        }
    }
}
```

## ⚡ Performance Considerations

- **'f' (fast) variants**: Faster signatures but larger size
- **'s' (small) variants**: Smaller signatures but slower
- **Recommendation**: Use SHAKE-128f for most applications

## 🛡️ Security

- ✅ **Quantum attack resistant** (Shor, Grover)
- ✅ **Hash function based** (SHAKE256)
- ✅ **Official FIPS 205 standard**
- ⚠️ **Large signatures**: 7KB - 49KB depending on scheme
- ⚠️ **One-time keys**: Do not reuse private keys

## 🔗 Repository and Source Code

```bibtex
@misc{revelacion1dev_slh_dsa_2024,
  author       = {revelacion1dev},
  title        = {{SLH-DSA}: Stateless Hash-Based Digital Signature Algorithm Implementation},
  year         = {2025},
  publisher    = {GitHub},
  journal      = {GitHub repository},
  howpublished = {\url{https://github.com/revelacion1dev/SLH-DSA}},
  note         = {Source code available at: \url{https://github.com/revelacion1dev/SLH-DSA}},
  url          = {https://github.com/revelacion1dev/SLH-DSA}
}
```
