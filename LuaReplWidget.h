#pragma once

#include <QPlainTextEdit>

namespace sol
{
  class state;
}

class LuaReplWidgetImpl;

class LuaReplWidget : public QPlainTextEdit
{
  Q_OBJECT

public:
  explicit LuaReplWidget(sol::state & lua, QWidget *parent = nullptr);
  ~LuaReplWidget();

  LuaReplWidget(LuaReplWidget const & other) = delete;
  LuaReplWidget & operator=(LuaReplWidget const & other) = delete;

  LuaReplWidget(LuaReplWidget && other);
  LuaReplWidget & operator=(LuaReplWidget && other);

protected:
  virtual void keyPressEvent(QKeyEvent *e) override;

private:
  LuaReplWidgetImpl * pimpl;
};
