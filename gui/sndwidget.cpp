#include "sndwidget.h"
#include "s2wcontext.h"
#include "synth/synthcontext.h"
#include "sndnode.h"
#include <QGridLayout>
#include <QLabel>
#include <QSpinBox>
#include <QtDebug>

SndWidget::SndWidget(QWidget* parent)
: QGroupBox(parent), ctx(nullptr), seq(nullptr)
{
  setTitle(tr("Variations"));
  setEnabled(false);

  QGridLayout* layout = new QGridLayout(this);

  QLabel* lSubsong = new QLabel(tr("Subsong:"), this);
  lSubsong->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
  subsong = new QSpinBox(this);
  subsong->setRange(0, 255);
  lSubsong->setBuddy(subsong);
  layout->addWidget(lSubsong, 0, 0);
  layout->addWidget(subsong, 0, 1);
  QObject::connect(subsong, SIGNAL(valueChanged(int)), this, SLOT(subsongUpdated(int)));

  QLabel* lExcite = new QLabel(tr("Excite:"), this);
  lExcite->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
  excite = new QSpinBox(this);
  excite->setRange(0, 255);
  lExcite->setBuddy(excite);
  layout->addWidget(lExcite, 0, 2);
  layout->addWidget(excite, 0, 3);
  QObject::connect(excite, SIGNAL(valueChanged(int)), this, SLOT(exciteUpdated(int)));

  QGroupBox* gRegs = new QGroupBox(tr("Registers"), this);
  QGridLayout* lRegs = new QGridLayout(gRegs);
  layout->addWidget(gRegs, 1, 0, 1, 4);

  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      int r = i * 4 + j;
      QLabel* lReg = new QLabel(QStringLiteral("%1:").arg(r), gRegs);
      lReg->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
      regs[r] = new QSpinBox(gRegs);
      regs[r]->setRange(0, 255);
      lReg->setBuddy(regs[r]);
      lRegs->addWidget(lReg, i, j * 2);
      lRegs->addWidget(regs[r], i, j * 2 + 1);
      QObject::connect(regs[r], qOverload<int>(&QSpinBox::valueChanged), [r, this](int v) { registerUpdated(r, v); });
    }
  }
}

void SndWidget::contextUpdated(SynthContext* ctx)
{
  this->ctx = ctx;
  setEnabled(!!ctx);

  if (ctx && ctx->channels.size()) {
    SndSequence* oldseq = seq;
    seq = reinterpret_cast<SndSequence*>(ctx->s2wContext()->pluginData);
    if (!seq) {
      setEnabled(false);
      return;
    }
    if (seq != oldseq) {
      int max = seq->numSubsongs() - 1;
      subsong->setRange(0, max < 0 ? 0 : max);
      subsong->setValue(seq->subsong());
      excite->setValue(seq->excite());
      for (int i = 0; i < 16; i++) {
        regs[i]->setValue(seq->getRegister(i));
      }
    }
  }
}

void SndWidget::subsongUpdated(int v)
{
  if (ctx && seq && isEnabled()) {
    seq->setSubsong(v);
    ctx->seek(0);
  }
}

void SndWidget::exciteUpdated(int v)
{
  if (ctx && seq && isEnabled()) {
    seq->setRegister(16, v);
  }
}

void SndWidget::registerUpdated(int r, int v)
{
  if (ctx && seq && isEnabled()) {
    seq->setRegister(r, v);
  }
}
