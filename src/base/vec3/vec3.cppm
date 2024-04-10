import vec3;

constexpr Vec3::Vec3(double x) noexcept : m_vec{ x, x, x } {}
constexpr Vec3::Vec3(double x, double y, double z) noexcept : m_vec{ x, y, z } {}

constexpr double Vec3::getX() const noexcept { return m_vec[0]; }
constexpr double Vec3::getY() const noexcept { return m_vec[1]; }
constexpr double Vec3::getZ() const noexcept { return m_vec[2]; }

const double& Vec3::operator[](const std::size_t index) const noexcept
{
	return m_vec[index];
}

double& Vec3::operator[](std::size_t index) noexcept {

	return const_cast<double&>(std::as_const(*this)[index]);
}

Vec3& Vec3::operator+=(const Vec3& otherVec) noexcept
{
	m_vec[0] += otherVec[0];
	m_vec[1] += otherVec[1];
	m_vec[2] += otherVec[2];

	return *this;
}

Vec3& Vec3::operator*=(double scalar) noexcept
{
	m_vec[0] *= scalar;
	m_vec[1] *= scalar;
	m_vec[2] *= scalar;

	return *this;
}

Vec3& Vec3::operator/=(double scalar) noexcept
{
	return ((*this) *= (1 / scalar));
}

constexpr Vec3 Vec3::operator-() const noexcept
{
	return Vec3(-m_vec[0], -m_vec[1], -m_vec[2]);
}

constexpr double Vec3::getMagnitudeSq() const noexcept
{
	return m_vec[0] * m_vec[0] + m_vec[1] * m_vec[1] + m_vec[2] * m_vec[2];
}

double Vec3::getMagnitude() const noexcept
{
	return std::sqrt(getMagnitudeSq());
}

// Non-member vec3 utils.
std::ostream& operator<<(std::ostream& outStream, const Vec3& outVec)
{
	return outStream << outVec[0] << " " << outVec[1] << " " << outVec[2] << "\n";
}

constexpr Vec3 operator+(const Vec3& f, const Vec3& s) noexcept
{
	return Vec3(f[0] + s[0], f[1] + s[1], f[2] + s[2]);
}

constexpr Vec3 operator-(const Vec3& f, const Vec3& s) noexcept
{
	return Vec3(f[0] - s[0], f[1] - s[1], f[2] - s[2]);
}

constexpr Vec3 operator*(const Vec3& f, const Vec3& s) noexcept
{
	return Vec3(f[0] * s[0], f[1] * s[1], f[2] * s[2]);
}

constexpr Vec3 operator*(double t, const Vec3& s) noexcept
{
	return Vec3(t * s[0], t * s[1], t * s[2]);
}

constexpr Vec3 operator*(const Vec3& f, double t) noexcept
{
	return t * f;
}

constexpr Vec3 operator/(const Vec3& f, double t) noexcept
{
	return (1 / t) * f;
}

double computeDot(const Vec3& f, const Vec3& s) noexcept
{
	return (f[0] * s[0]) + (f[1] * s[1]) + (f[2] * s[2]);
}

Vec3 computeCross(const Vec3& f, const Vec3& s) noexcept
{
	return Vec3(f[1] * s[2] - f[2] * s[1],
		f[2] * s[0] - f[0] * s[2],
		f[0] * s[1] - f[1] * s[0]);
}

Vec3 getUnit(const Vec3& inputVec) noexcept
{
	return inputVec / inputVec.getMagnitude();
}
