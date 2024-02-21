#include <QApplication>
#include "mainwindow.h"
#include "sndwidget.h"
#include "clefcontext.h"
#include "synth/synthcontext.h"
#include "plugin/baseplugin.h"
#include <QtDebug>

class S989Window : public MainWindow
{
public:
  S989Window(ClefPluginBase* plugin) : MainWindow(plugin) {}

protected:
  virtual QWidget* createPluginWidget(QWidget* parent) override {
    SndWidget* w = new SndWidget(parent);
    lockWhileWorking(w);
    return w;
  }
};

int main(int argc, char** argv)
{
  ClefContext ctx;
  ClefPluginBase* plugin = Clef::makePlugin(&ctx);

  QCoreApplication::setApplicationName(QString::fromStdString(plugin->pluginName()));
  QCoreApplication::setApplicationVersion(QString::fromStdString(plugin->version()));
  QCoreApplication::setOrganizationName("libclef");
  QCoreApplication::setOrganizationDomain("libclef" + QString::fromStdString(plugin->pluginShortName()));
  QApplication app(argc, argv);

  S989Window mw(plugin);
  mw.show();
  if (app.arguments().length() > 1) {
    mw.openFile(app.arguments()[1], true);
  }

  return app.exec();
}
