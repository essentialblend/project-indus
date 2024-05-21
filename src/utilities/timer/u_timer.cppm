import u_timer;

UTimer::UTimer(std::string_view name) noexcept : m_timerName{ name }, m_timerID{ generateTimerID() } {}

int UTimer::generateTimerID() noexcept
{
	static int timerID{ 0 };
	return timerID++;
}

void UTimer::startEntity()
{
	m_startTime = std::chrono::high_resolution_clock::now();
	m_isTimerRunning = true;
}

void UTimer::stopEntity()
{
	if (m_isTimerRunning)
	{
		m_endTime = std::chrono::high_resolution_clock::now();
		m_isTimerRunning = false;
	}
}

void UTimer::resetTimer() noexcept
{
	m_startTime = std::chrono::time_point<std::chrono::high_resolution_clock>{};
	m_endTime = std::chrono::time_point<std::chrono::high_resolution_clock>{};
	m_isTimerRunning = false;
}

std::chrono::duration<double> UTimer::getElapsedTime() const noexcept
{
	const auto endTime{ m_isTimerRunning ? std::chrono::high_resolution_clock::now() : m_endTime };
	return (endTime - m_startTime);
}

std::string_view UTimer::getTimerName() const noexcept
{
	return m_timerName;
}

std::string_view UTimer::getTimerResultString() const
{
	auto elapsedSeconds{ std::chrono::duration_cast<std::chrono::seconds>(getElapsedTime()) };
	auto elapsedMinutes{ std::chrono::duration_cast<std::chrono::minutes>(elapsedSeconds) };
	auto finalSeconds{ elapsedSeconds.count() - (static_cast<long long>(elapsedMinutes.count()) * 60) };
	return std::format("{} min {} sec", elapsedMinutes.count(), finalSeconds);
}

int UTimer::getTimerID() const noexcept
{
	return m_timerID;
}