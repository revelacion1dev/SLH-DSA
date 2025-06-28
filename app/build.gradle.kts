plugins {
    alias(libs.plugins.android.library)
    alias(libs.plugins.kotlin.android)
}

android {
    namespace = "com.revelacion1.slh_dsa_library"
    compileSdk = 35

    defaultConfig {
        minSdk = 26
        externalNativeBuild {
            cmake {
                cppFlags += "-std=c++17"
                arguments += "-DANDROID_STL=c++_shared"
            }
        }
    }

    // CONFIGURACIÓN DE SABORES POR ARQUITECTURA
    flavorDimensions += "architecture"

    productFlavors {
        create("arm64") {
            dimension = "architecture"
            ndk {
                abiFilters.clear()
                abiFilters += "arm64-v8a"  // Solo ARM64 (~1.8MB)
            }
        }

        create("arm32") {
            dimension = "architecture"
            ndk {
                abiFilters.clear()
                abiFilters += "armeabi-v7a"  // Solo ARM32 (~1.5MB)
            }
        }

        create("x86") {
            dimension = "architecture"
            ndk {
                abiFilters.clear()
                abiFilters += "x86"  // Solo x86 (~1.7MB)
            }
        }

        create("x64") {
            dimension = "architecture"
            ndk {
                abiFilters.clear()
                abiFilters += "x86_64"  // Solo x86_64 (~1.9MB)
            }
        }

        create("universal") {
            dimension = "architecture"
            ndk {
                abiFilters.clear()
                abiFilters += listOf("arm64-v8a", "armeabi-v7a", "x86", "x86_64")  // Todas (~7MB)
            }
        }

        create("mobile") {
            dimension = "architecture"
            ndk {
                abiFilters.clear()
                abiFilters += listOf("arm64-v8a", "armeabi-v7a")  // Solo móviles (~3.3MB)
            }
        }
    }

    buildTypes {
        release {
            isMinifyEnabled = true
            proguardFiles(
                getDefaultProguardFile("proguard-android-optimize.txt"),
                "proguard-rules.pro"
            )
        }
    }

    compileOptions {
        sourceCompatibility = JavaVersion.VERSION_11
        targetCompatibility = JavaVersion.VERSION_11
    }

    kotlinOptions {
        jvmTarget = "11"
    }

    externalNativeBuild {
        cmake {
            path = file("src/main/cpp/CMakeLists.txt")
            version = "3.22.1"
        }
    }

    publishing {
        singleVariant("release") {
            withSourcesJar()
            withJavadocJar()
        }
    }
}

dependencies {
    implementation(libs.androidx.core.ktx)
}