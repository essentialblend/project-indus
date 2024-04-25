export module timer;

import <chrono>;
import <print>;

export class Timer
{
public:
	explicit Timer() noexcept = default;

	void startTimer() noexcept;
	void endTimer() noexcept;
	std::chrono::duration<double> getElapsedTime() const noexcept;
	std::string getTimerResultString() const;
	void resetTimer() noexcept;

private:
	std::chrono::time_point<std::chrono::high_resolution_clock> m_startTime{};
	std::chrono::time_point<std::chrono::high_resolution_clock> m_endTime{};
	bool hasTimerStopped{ false };
};

void Timer::startTimer() noexcept
{
	m_startTime = std::chrono::high_resolution_clock::now();
}

void Timer::endTimer() noexcept
{
	if(!hasTimerStopped)
		m_endTime = std::chrono::high_resolution_clock::now();
		hasTimerStopped = true;
}

std::chrono::duration<double> Timer::getElapsedTime() const noexcept
{
	const auto endTime{ hasTimerStopped ? m_endTime : std::chrono::high_resolution_clock::now() };
	return (endTime - m_startTime);
}

std::string Timer::getTimerResultString() const
{
	auto elapsedSeconds{ std::chrono::duration_cast<std::chrono::seconds>(getElapsedTime()) };
	auto elapsedMinutes{ std::chrono::duration_cast<std::chrono::minutes>(elapsedSeconds) };
	auto finalSeconds{ elapsedSeconds.count() - (static_cast<long long>(elapsedMinutes.count()) * 60) };
	return std::format("{} min {} sec", elapsedMinutes.count(), finalSeconds);
}

void Timer::resetTimer() noexcept
{
	m_startTime = std::chrono::time_point<std::chrono::high_resolution_clock>{};
	m_endTime = std::chrono::time_point<std::chrono::high_resolution_clock>{};
	hasTimerStopped = false;
}