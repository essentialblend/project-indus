export module displaysink;

import engineconstructs;

export class DisplaySink
{
public:
  virtual ~DisplaySink() noexcept = default;
  virtual void present() = 0;
  virtual void update(const FrameSnapshot& frame) = 0;
  virtual bool isSinkOpen() const noexcept = 0;
};

