export module es_pathtracer;

import i_enginesystem;

export class ESPathTracer : public IEngineSystem
{
public:
	explicit ESPathTracer() noexcept = default;
	ESPathTracer(const ESPathTracer&) = delete;
	ESPathTracer& operator=(const ESPathTracer&) = delete;	
	ESPathTracer(ESPathTracer&&) noexcept = default;	
	ESPathTracer& operator=(ESPathTracer&&) noexcept = default;

	virtual void initializeEntity() override;

	bool isRenderComplete() const noexcept;

	~ESPathTracer() noexcept = default;

private:

};