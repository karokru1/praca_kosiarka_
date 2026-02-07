package com.example.kosiarka.data

import kotlinx.serialization.Serializable
import kotlinx.serialization.json.Json
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.withContext
import okhttp3.MediaType.Companion.toMediaType
import okhttp3.OkHttpClient
import okhttp3.logging.HttpLoggingInterceptor
import retrofit2.Retrofit
import com.jakewharton.retrofit2.converter.kotlinx.serialization.asConverterFactory
import retrofit2.http.Body
import retrofit2.http.GET
import retrofit2.http.Header
import retrofit2.http.POST
import java.util.concurrent.TimeUnit

@Serializable
data class StatusDto(
    val temperature: Float = 0f,
    val humidity: Float = 0f,
    val pressure: Float = 0f,
    val light: Float = 0f,
    val soilMoisture: Float = 0f,
    val grassHeight: Float = -1f,
    val rainDetected: Boolean = false,
    val targetGrassHeight: Float = 10f
)

@Serializable
data class TargetBody(val heightCm: Float)

@Serializable
data class HistoryItem(
    val timestamp: String,
    val status: StatusDto
)

interface KosiarkaApi {
    @GET("status")
    suspend fun status(@Header("Authorization") token: String? = null): StatusDto

    @POST("target")
    suspend fun setTarget(
        @Body body: TargetBody,
        @Header("Authorization") token: String? = null
    )

    @POST("mow/start")
    suspend fun startMow(@Header("Authorization") token: String? = null)

    @GET("history")
    suspend fun history(@Header("Authorization") token: String? = null): List<HistoryItem>
}

object ApiFactory {
    private val json = Json { ignoreUnknownKeys = true; coerceInputValues = true }

    fun create(baseUrl: String, token: String?): KosiarkaApi {
        val logging = HttpLoggingInterceptor().apply { level = HttpLoggingInterceptor.Level.BASIC }
        val client = OkHttpClient.Builder()
            .addInterceptor(logging)
            .connectTimeout(10, TimeUnit.SECONDS)
            .readTimeout(15, TimeUnit.SECONDS)
            .build()

        val contentType = "application/json".toMediaType()
        val retrofit = Retrofit.Builder()
            .baseUrl(normalizeBaseUrl(baseUrl))
            .client(client)
            .addConverterFactory(json.asConverterFactory(contentType))
            .build()

        return retrofit.create(KosiarkaApi::class.java)
    }

    private fun normalizeBaseUrl(input: String): String {
        val trimmed = input.trim()
        return if (trimmed.endsWith("/")) trimmed else "$trimmed/"
    }
}

class KosiarkaRepository(private val api: KosiarkaApi, private val token: String?) {
    suspend fun fetchStatus(): Result<StatusDto> = runCatching {
        withContext(Dispatchers.IO) { api.status(authHeader()) }
    }

    suspend fun setTarget(height: Float): Result<Unit> = runCatching {
        val clamped = height.coerceIn(5f, 15f)
        withContext(Dispatchers.IO) { api.setTarget(TargetBody(clamped), authHeader()) }
    }

    suspend fun startMow(): Result<Unit> = runCatching {
        withContext(Dispatchers.IO) { api.startMow(authHeader()) }
    }

    suspend fun fetchHistory(): Result<List<HistoryItem>> = runCatching {
        withContext(Dispatchers.IO) { api.history(authHeader()) }
    }

    private fun authHeader(): String? = token?.takeIf { it.isNotBlank() }?.let { "Bearer $it" }
}
