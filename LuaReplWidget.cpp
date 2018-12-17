#include "LuaReplWidget.h"

#include <sol.hpp>

#include <QTextBlock>
#include <QTextCursor>

namespace
{
  QString const Prompt1 = QStringLiteral(">>> ");
  QString const Prompt2 = QStringLiteral("... ");
  int const PromptLength = Prompt1.size();
}

class LuaReplWidgetImpl
{
public:
  LuaReplWidgetImpl(sol::state & lua_, QPlainTextEdit & textEdit_);

  bool keyPressEvent(QKeyEvent *e);

private:
  void print(sol::variadic_args & args);
  void prompt();
  void prompt(std::string const & message);
  void addHistory(std::string const & line);
  void restore(std::string const & line = {});

  using HistoryContainer = std::vector<std::string>;
  using HistoryIndex = std::size_t;

  sol::state & lua;
  QPlainTextEdit & textEdit;
  HistoryContainer history;
  HistoryIndex historyIndex;
  std::string command;
  bool multiline;
};

LuaReplWidgetImpl::LuaReplWidgetImpl(sol::state & lua_, QPlainTextEdit & textEdit_)
  : lua(lua_)
  , textEdit(textEdit_)
  , history()
  , historyIndex(0)
  , command()
  , multiline(false)
{
  prompt();
  lua.set_function("print", &LuaReplWidgetImpl::print, this);
}

bool LuaReplWidgetImpl::keyPressEvent(QKeyEvent *e)
{
  auto cursor = textEdit.textCursor();
  auto const cursorBlock = cursor.blockNumber();
  auto const maxBlock = textEdit.blockCount() - 1;
  bool passEventToParent = false;

  switch(e->key())
  {
  case Qt::Key_Enter:
  case Qt::Key_Return:
  {
    auto block = cursor.block();
    auto line = block.text().remove(0, PromptLength).toStdString();
    auto chunk = lua.load(command += line + "\n");

    addHistory(line);

    switch(chunk.status())
    {
    case sol::call_status::ok:
    {
      auto result = chunk();

      multiline = false;

      if(result.valid())
      {
        for(auto const & value : result)
        {
          textEdit.appendPlainText(QString::fromStdString(value));
        }

        prompt();
      }
      else
      {
        prompt(result.get<std::string>());
      }
    }
    break;

    case sol::call_status::syntax:
    {
      char constexpr EofMarker[] = "'<eof>'";
      auto constexpr len = sizeof EofMarker - 1;
      std::string message = chunk;
      auto const pos = message.size() - len;

      if(0 == message.compare(pos, len, EofMarker))
      {
        multiline = true;
        prompt();
      }
      else
      {
        multiline = false;
        prompt(message);
      }
    }
    break;

    default:
      multiline = false;
      prompt(chunk.get<std::string>());
      break;
    }
  }
  break;

  case Qt::Key_Up:
    if(historyIndex > 0 && !history.empty())
    {
      if(--historyIndex >= history.size())
        historyIndex = history.size() - 1;

      restore(history[historyIndex]);
    }
    break;

  case Qt::Key_Down:
    if(++historyIndex < history.size())
      restore(history[historyIndex]);
    else
      restore();
    break;

  case Qt::Key_Backspace:
    if(cursorBlock == maxBlock && cursor.positionInBlock() > PromptLength)
      passEventToParent = true;
    break;

  case Qt::Key_Home:
    cursor.movePosition(QTextCursor::End);
    cursor.movePosition(QTextCursor::StartOfBlock);
    cursor.setPosition(cursor.position() + PromptLength);
    textEdit.setTextCursor(cursor);
    break;

  default:
    if(cursorBlock != maxBlock)
      textEdit.moveCursor(QTextCursor::End);
    passEventToParent = true;
    break;
  }

  return passEventToParent;
}

void LuaReplWidgetImpl::print(sol::variadic_args & args)
{
  for(auto & arg : args)
    textEdit.appendPlainText(QString::fromStdString(arg));
}

void LuaReplWidgetImpl::prompt()
{
  if(multiline)
  {
    textEdit.appendPlainText(Prompt2);
  }
  else
  {
    textEdit.appendPlainText(Prompt1);
    command.clear();
  }
  textEdit.moveCursor(QTextCursor::End);
}

void LuaReplWidgetImpl::prompt(std::string const & message)
{
  textEdit.appendPlainText(QString::fromStdString(message));
  prompt();
}

void LuaReplWidgetImpl::addHistory(std::string const & line)
{
  history.push_back(line);
  historyIndex += 1;
}

void LuaReplWidgetImpl::restore(std::string const & input)
{
  auto cursor = textEdit.textCursor();
  auto const line =
    (multiline ? Prompt2 : Prompt1) +
    QString::fromStdString(input);

  cursor.movePosition(QTextCursor::End);
  cursor.select(QTextCursor::BlockUnderCursor);
  cursor.removeSelectedText();
  textEdit.appendPlainText(line);
}

LuaReplWidget::LuaReplWidget(sol::state & lua_, QWidget *parent)
  : QPlainTextEdit(parent)
  , pimpl(new LuaReplWidgetImpl(lua_, *this))
{
  setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
}

LuaReplWidget::LuaReplWidget(LuaReplWidget && other)
  : pimpl(other.pimpl)
{
  other.pimpl = nullptr;
}

LuaReplWidget & LuaReplWidget::operator=(LuaReplWidget && other)
{
  delete pimpl;
  new (this) LuaReplWidget(std::move(other));

  return *this;
}

LuaReplWidget::~LuaReplWidget()
{
  delete pimpl;
}

void LuaReplWidget::keyPressEvent(QKeyEvent *e)
{
  if(pimpl->keyPressEvent(e))
    QPlainTextEdit::keyPressEvent(e);
}
