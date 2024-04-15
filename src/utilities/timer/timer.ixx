export module timer;

import <chrono>;
import <print>;

export class Timer
{
public:
	explicit Timer() noexcept = default;

	void startTimer() noexcept
	{
		m_startTime = std::chrono::high_resolution_clock::now();
	}

	void endTimer() noexcept
	{
		m_endTime = std::chrono::high_resolution_clock::now();
	}

	std::chrono::duration<double> getElapsedTime() const noexcept
	{
		return (m_endTime - m_startTime);
	}

	std::string getTimerResultString() const
	{
		auto elapsedSeconds{ std::chrono::duration_cast<std::chrono::seconds>(getElapsedTime()) };
		auto elapsedMinutes{ std::chrono::duration_cast<std::chrono::minutes>(elapsedSeconds)};
		auto finalSeconds{ elapsedSeconds.count() - (static_cast<long long>(elapsedMinutes.count()) * 60) };
		return std::format("{} min {} sec", elapsedMinutes.count(), finalSeconds);
	}

	void resetTimer() noexcept
	{
		m_startTime = std::chrono::time_point<std::chrono::high_resolution_clock>{};
		m_endTime = std::chrono::time_point<std::chrono::high_resolution_clock>{};
	}

private:
	std::chrono::time_point<std::chrono::high_resolution_clock> m_startTime{};
	std::chrono::time_point<std::chrono::high_resolution_clock> m_endTime{};
};