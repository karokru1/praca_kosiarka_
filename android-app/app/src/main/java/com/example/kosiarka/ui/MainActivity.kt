package com.example.kosiarka.ui

import android.os.Bundle
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.activity.viewModels
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.verticalScroll
import androidx.compose.material3.Button
import androidx.compose.material3.Card
import androidx.compose.material3.ExperimentalMaterial3Api
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Scaffold
import androidx.compose.material3.Slider
import androidx.compose.material3.Text
import androidx.compose.material3.TextButton
import androidx.compose.material3.TopAppBar
import androidx.compose.runtime.Composable
import androidx.compose.runtime.collectAsState
import androidx.compose.runtime.getValue
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.unit.dp
import androidx.lifecycle.ViewModel
import androidx.lifecycle.ViewModelProvider
import androidx.lifecycle.viewModelScope
import androidx.lifecycle.viewmodel.compose.viewModel
import com.example.kosiarka.data.ApiFactory
import com.example.kosiarka.data.HistoryItem
import com.example.kosiarka.data.KosiarkaRepository
import com.example.kosiarka.data.StatusDto
import kotlinx.coroutines.flow.MutableStateFlow
import kotlinx.coroutines.flow.StateFlow
import kotlinx.coroutines.flow.update
import kotlinx.coroutines.launch

private const val DEFAULT_BASE_URL = "http://raspberrypi.local:8000/"

class MainActivity : ComponentActivity() {
    private val viewModel: MainViewModel by viewModels { MainViewModel.Factory }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContent { KosiarkaApp(viewModel) }
    }
}

data class UiState(
    val isLoading: Boolean = false,
    val error: String? = null,
    val status: StatusDto? = null,
    val history: List<HistoryItem> = emptyList(),
    val baseUrl: String = DEFAULT_BASE_URL,
    val token: String = "",
    val targetInput: Float = 10f
)

class MainViewModel(
    private val repoBuilder: (String, String?) -> KosiarkaRepository
) : ViewModel() {
    private val _uiState = MutableStateFlow(UiState())
    val uiState: StateFlow<UiState> = _uiState

    fun updateBaseUrl(value: String) {
        _uiState.update { it.copy(baseUrl = value) }
    }

    fun updateToken(value: String) {
        _uiState.update { it.copy(token = value) }
    }

    fun updateTarget(value: Float) {
        _uiState.update { it.copy(targetInput = value.coerceIn(5f, 15f)) }
    }

    fun refresh() {
        viewModelScope.launch {
            val state = _uiState.value
            val repo = repoBuilder(state.baseUrl, state.token.ifBlank { null })
            _uiState.update { it.copy(isLoading = true, error = null) }
            val statusResult = repo.fetchStatus()
            val historyResult = repo.fetchHistory()
            _uiState.update {
                it.copy(
                    isLoading = false,
                    status = statusResult.getOrNull(),
                    history = historyResult.getOrDefault(emptyList()),
                    error = statusResult.exceptionOrNull()?.message
                        ?: historyResult.exceptionOrNull()?.message
                )
            }
        }
    }

    fun saveTarget() {
        viewModelScope.launch {
            val state = _uiState.value
            val repo = repoBuilder(state.baseUrl, state.token.ifBlank { null })
            _uiState.update { it.copy(isLoading = true, error = null) }
            val result = repo.setTarget(state.targetInput)
            _uiState.update {
                it.copy(
                    isLoading = false,
                    error = result.exceptionOrNull()?.message
                )
            }
        }
    }

    fun startMow() {
        viewModelScope.launch {
            val state = _uiState.value
            val repo = repoBuilder(state.baseUrl, state.token.ifBlank { null })
            _uiState.update { it.copy(isLoading = true, error = null) }
            val result = repo.startMow()
            _uiState.update {
                it.copy(
                    isLoading = false,
                    error = result.exceptionOrNull()?.message
                )
            }
        }
    }

    companion object {
        val Factory: ViewModelProvider.Factory = object : ViewModelProvider.Factory {
            override fun <T : ViewModel> create(modelClass: Class<T>): T {
                val builder: (String, String?) -> KosiarkaRepository = { baseUrl, token ->
                    KosiarkaRepository(ApiFactory.create(baseUrl, token), token)
                }
                @Suppress("UNCHECKED_CAST")
                return MainViewModel(builder) as T
            }
        }
    }
}

@Composable
@OptIn(ExperimentalMaterial3Api::class)
fun KosiarkaApp(viewModel: MainViewModel = viewModel(factory = MainViewModel.Factory)) {
    val state by viewModel.uiState.collectAsState()

    MaterialTheme {
        Scaffold(
            topBar = {
                TopAppBar(title = { Text("Kosiarka Monitor") }, actions = {
                    TextButton(onClick = { viewModel.refresh() }) { Text("Odswiez") }
                })
            }
        ) { padding ->
            Column(
                modifier = Modifier
                    .padding(padding)
                    .padding(16.dp)
                    .fillMaxSize()
                    .verticalScroll(rememberScrollState()),
                verticalArrangement = Arrangement.spacedBy(16.dp)
            ) {
                TargetSection(
                    value = state.targetInput,
                    onChange = viewModel::updateTarget,
                    onSave = viewModel::saveTarget
                )

                StatusSection(status = state.status)
                HistorySection(history = state.history)

                if (state.error != null) {
                    Text(text = "Blad: ${state.error}", color = MaterialTheme.colorScheme.error)
                }

                if (state.isLoading) {
                    Text("Ladowanie...")
                }
            }
        }
    }
}

@Composable
private fun TargetSection(
    value: Float,
    onChange: (Float) -> Unit,
    onSave: () -> Unit
) {
    Card(modifier = Modifier.fillMaxWidth()) {
        Column(modifier = Modifier.padding(16.dp), verticalArrangement = Arrangement.spacedBy(12.dp)) {
            Text("Wysokosc trawy (5-15 cm)", style = MaterialTheme.typography.titleMedium)
            Slider(
                value = value,
                onValueChange = onChange,
                valueRange = 5f..15f
            )
            Row(verticalAlignment = Alignment.CenterVertically) {
                Text(String.format("%.1f cm", value))
                Spacer(modifier = Modifier.weight(1f))
                Button(onClick = onSave) { Text("Zapisz") }
            }
        }
    }
}

@Composable
private fun StatusSection(status: StatusDto?) {
    Card(modifier = Modifier.fillMaxWidth()) {
        Column(modifier = Modifier.padding(16.dp), verticalArrangement = Arrangement.spacedBy(6.dp)) {
            Text("Status", style = MaterialTheme.typography.titleMedium)
            if (status == null) {
                Text("Brak danych.")
            } else {
                val decision = evaluateDecision(status)
                StatusRow("Temperatura", "${status.temperature} C")
                StatusRow("Wilgotnosc", "${status.humidity} %")
                StatusRow("Cisnienie", "${status.pressure} hPa")
                StatusRow("Swiatlo", "${status.light} lux")
                StatusRow("Wilgotnosc gleby", "${status.soilMoisture} %")
                StatusRow("Deszcz", if (status.rainDetected) "TAK" else "NIE")
                StatusRow("Wysokosc trawy", "${status.grassHeight} cm")
                StatusRow("Decyzja", if (decision.canMow) "KOSIC" else "NIE KOSIC")
                StatusRow("Powod", decision.reason)
            }
        }
    }
}

@Composable
private fun StatusRow(label: String, value: String) {
    Row(modifier = Modifier.fillMaxWidth(), horizontalArrangement = Arrangement.SpaceBetween) {
        Text(label)
        Text(value, style = MaterialTheme.typography.bodyMedium)
    }
}

@Composable
private fun HistorySection(history: List<HistoryItem>) {
    Card(modifier = Modifier.fillMaxWidth()) {
        Column(modifier = Modifier.padding(16.dp), verticalArrangement = Arrangement.spacedBy(8.dp)) {
            Text("Historia odczytow", style = MaterialTheme.typography.titleMedium)
            if (history.isEmpty()) {
                Text("Brak danych.")
            } else {
                history.take(10).forEach { item ->
                    Card(modifier = Modifier.fillMaxWidth()) {
                        Column(modifier = Modifier.padding(12.dp), verticalArrangement = Arrangement.spacedBy(4.dp)) {
                            Text(item.timestamp, style = MaterialTheme.typography.labelMedium)
                            StatusRow("Temp", "${item.status.temperature} C")
                            StatusRow("Wilgotnosc", "${item.status.humidity} %")
                            StatusRow("Cisnienie", "${item.status.pressure} hPa")
                            StatusRow("Swiatlo", "${item.status.light} lux")
                            StatusRow("Trawa", "${item.status.grassHeight} cm")
                            StatusRow("Deszcz", if (item.status.rainDetected) "TAK" else "NIE")
                        }
                    }
                }
            }
        }
    }
}

private data class DecisionUi(val canMow: Boolean, val reason: String)

private fun evaluateDecision(data: StatusDto): DecisionUi {
    if (data.rainDetected) {
        return DecisionUi(false, "deszcz wykryty")
    }
    if (data.soilMoisture > 800f) {
        return DecisionUi(false, "wilgotnosc gleby powyzej progu")
    }
    if (data.humidity > 90f) {
        return DecisionUi(false, "wilgotnosc powietrza powyzej progu")
    }
    if (data.light < 100f) {
        return DecisionUi(false, "za malo swiatla")
    }
    if (data.light > 44000f) {
        return DecisionUi(false, "za duze natezenie swiatla")
    }
    if (data.temperature < 10f || data.temperature > 35f) {
        return DecisionUi(false, "temperatura poza zakresem")
    }
    if (data.grassHeight < 0f) {
        return DecisionUi(false, "brak pomiaru wysokosci trawy")
    }
    if (data.grassHeight < 5f) {
        return DecisionUi(false, "trawa za niska")
    }
    if (data.grassHeight >= 15f) {
        return DecisionUi(true, "trawa przekroczyla prog")
    }
    return DecisionUi(false, "trawa ponizej progu koszenia")
}
