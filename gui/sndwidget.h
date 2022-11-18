#ifndef S989_WIDGET_H
#define S989_WIDGET_H

#include <QGroupBox>
class SynthContext;
class SndSequence;
class QSpinBox;

class SndWidget : public QGroupBox
{
Q_OBJECT
public:
  SndWidget(QWidget* parent = nullptr);

public slots:
  void contextUpdated(SynthContext* ctx);

private slots:
  void subsongUpdated(int);
  void exciteUpdated(int);
  void registerUpdated(int, int);

private:
  SynthContext* ctx;
  SndSequence* seq;

  QSpinBox* subsong;
  QSpinBox* excite;
  QSpinBox* regs[16];
};

#endif
