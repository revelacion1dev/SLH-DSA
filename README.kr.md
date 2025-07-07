# SLH-DSA Android 라이브러리

**FIPS 205**에 따른 **SLH-DSA (Stateless Hash-Based Digital Signature Algorithm)** 표준을 기반으로 한 양자 후 디지털 서명을 위한 네이티브 Android 라이브러리입니다.

## 🔐 특징

- **SHAKE 변형을 포함한 완전한 FIPS 205 구현**
- **6가지 매개변수 스키마** 지원 (128s/f, 192s/f, 256s/f)
- **크로스 플랫폼 Android** (ARM32, ARM64, x86, x64)
- **사용하기 쉬운 Kotlin/Java API**
- **양자 컴퓨팅에 저항력**
- **외부 종속성 없음**

## 📦 지원되는 아키텍처

| AAR 파일 | 아키텍처 | 크기 | 설명 |
|----------|----------|------|------|
| `SLHDSA-arm32-release.aar` | ARMv7 (32비트) | 1.681 KB | 32비트 ARM 장치 |
| `SLHDSA-arm64-release.aar` | ARM64 (64비트) | 1.844 KB | 64비트 ARM 장치 |
| `SLHDSA-x64-release.aar` | x86_64 | 1.989 KB | x64 에뮬레이터 및 장치 |
| `SLHDSA-x86-release.aar` | x86 (32비트) | 1.921 KB | x86 에뮬레이터 |
| `SLHDSA-mobile-release.aar` | ARM32 + ARM64 | 3.519 KB | 모바일 장치만 |
| `SLHDSA-universal-release.aar` | 전체 | 7.416 KB | **권장**: 모든 아키텍처 |

## 🚀 설치

### 옵션 1: 범용 AAR 
```gradle
// app/build.gradle에서
dependencies {
    implementation files('libs/SLHDSA-universal-release.aar')
}
```

### 옵션 2: 특정 아키텍처
```gradle
// ARM64 장치만을 위한 설정
dependencies {
    implementation files('libs/SLHDSA-arm64-release.aar')
}
```

### 프로젝트 구성
1. 해당 `.aar` 파일을 `app/libs/` 폴더에 복사
2. `build.gradle`에 종속성 추가
3. 프로젝트 동기화

## 📚 기본 사용법

### 초기화
```kotlin
import com.revelacion1.slh_dsa_library.SLHDSA

val slhDsa = SLHDSA()

// SLH-DSA-SHAKE-128s 스키마로 초기화
if (slhDsa.initializeConfig(0)) {
    println("라이브러리가 성공적으로 초기화되었습니다")
} else {
    println("초기화 오류")
}
```

### 키 생성
```kotlin
val keyPair = slhDsa.slhKeyGen()
if (keyPair != null) {
    val publicKey = keyPair[0]   // 공개키
    val privateKey = keyPair[1]  // 개인키
    
    println("키 생성됨 - PK: ${publicKey.size} 바이트, SK: ${privateKey.size} 바이트")
}
```

### 메시지 서명
```kotlin
val message = "양자 후 세계 안녕하세요!".toByteArray()
val context = byteArrayOf() // 빈 컨텍스트

val signature = slhDsa.slhSign(message, context, privateKey)
if (signature != null) {
    println("메시지 서명됨 - 서명: ${signature.size} 바이트")
}
```

### 서명 검증
```kotlin
val isValid = slhDsa.slhVerify(message, signature, context, publicKey)
if (isValid) {
    println("✅ 유효한 서명")
} else {
    println("❌ 무효한 서명")
}
```

## 🔧 매개변수 스키마

| ID | 스키마 | 보안 수준 | PK 크기 | 서명 크기 | 속도 |
|----|--------|-----------|---------|-----------|------|
| 0 | SLH-DSA-SHAKE-128s | 128비트 | 32바이트 | ~7KB | 느림 |
| 1 | SLH-DSA-SHAKE-128f | 128비트 | 32바이트 | ~17KB | **빠름** |
| 2 | SLH-DSA-SHAKE-192s | 192비트 | 48바이트 | ~16KB | 느림 |
| 3 | SLH-DSA-SHAKE-192f | 192비트 | 48바이트 | ~35KB | **빠름** |
| 4 | SLH-DSA-SHAKE-256s | 256비트 | 64바이트 | ~29KB | 느림 |
| 5 | SLH-DSA-SHAKE-256f | 256비트 | 64바이트 | ~49KB | **빠름** |

### 스키마 변경
```kotlin
// SLH-DSA-SHAKE-256f로 변경 (최대 보안, 빠름)
if (slhDsa.setParameterScheme(5)) {
    val info = slhDsa.getCurrentSchemaInfo()
    println("현재 스키마: ${info?.name}")
    println("보안 수준: ${info?.security_category}")
}
```

## 📋 완전한 예제

```kotlin
class MainActivity : AppCompatActivity() {
    private lateinit var slhDsa: SLHDSA
    
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        
        // SLH-DSA 초기화
        slhDsa = SLHDSA()
        
        if (!slhDsa.initializeConfig(1)) { // SLH-DSA-SHAKE-128f
            Log.e("SLH-DSA", "라이브러리 초기화 오류")
            return
        }
        
        // 키 생성
        val keyPair = slhDsa.slhKeyGen() ?: run {
            Log.e("SLH-DSA", "키 생성 오류")
            return
        }
        
        val publicKey = keyPair[0]
        val privateKey = keyPair[1]
        
        // 문서 서명
        val document = "중요한 계약서".toByteArray()
        val context = "app_v1.0".toByteArray()
        
        val signature = slhDsa.slhSign(document, context, privateKey) ?: run {
            Log.e("SLH-DSA", "문서 서명 오류")
            return
        }
        
        // 서명 검증
        val isValid = slhDsa.slhVerify(document, signature, context, publicKey)
        
        if (isValid) {
            Log.i("SLH-DSA", "✅ 인증된 문서")
        } else {
            Log.w("SLH-DSA", "❌ 손상된 문서")
        }
        
        // 스키마 정보 표시
        slhDsa.getCurrentSchemaInfo()?.let { info ->
            Log.i("SLH-DSA", """
                스키마: ${info.name}
                보안: ${info.security_category} 비트
                PK 크기: ${info.pk_bytes} 바이트
                서명 크기: ${info.sig_bytes} 바이트
            """.trimIndent())
        }
    }
}
```

## ⚡ 성능 고려사항

- **'f' (빠름) 변형**: 더 빠른 서명이지만 더 큰 크기
- **'s' (작음) 변형**: 더 작은 서명이지만 더 느림
- **권장사항**: 대부분의 애플리케이션에 SHAKE-128f 사용

## 🛡️ 보안

- ✅ **양자 공격에 저항력** (Shor, Grover)
- ✅ **해시 함수 기반** (SHAKE256)
- ✅ **공식 FIPS 205 표준**
- ⚠️ **큰 서명**: 스키마에 따라 7KB - 49KB
- ⚠️ **일회성 키**: 개인키 재사용 금지

## 🔗 저장소 및 소스 코드

```bibtex
@misc{revelacion1dev_slh_dsa_2024,
  author       = {revelacion1dev},
  title        = {{SLH-DSA}: Stateless Hash-Based Digital Signature Algorithm Implementation},
  year         = {2025},
  publisher    = {GitHub},
  journal      = {GitHub repository},
  howpublished = {\url{https://github.com/revelacion1dev/SLH-DSA}},
  note         = {소스 코드 사용 가능 위치: \url{https://github.com/revelacion1dev/SLH-DSA}},
  url          = {https://github.com/revelacion1dev/SLH-DSA}
}
```
