
#include <flan/pcre_cheatsheet_dialog.hpp>
#include <QDialogButtonBox>
#include <QFile>
#include <QTextEdit>
#include <QVBoxLayout>

// Q_INIT_RESOURCE has to be called from the global namespace, hence this workaround.
inline void init_resource()
{
    Q_INIT_RESOURCE(libflan);
}

namespace
{
QByteArray get_pcre_cheatsheet_content()
{
    init_resource();
    QFile file(":/pcre_cheatsheet");
    file.open(QIODeviceBase::ReadOnly);
    auto data = file.readAll();
    file.close();

    return data;
}
} // namespace

namespace flan
{
pcre_cheatsheet_dialog_t::pcre_cheatsheet_dialog_t(QWidget* parent)
    : QDialog{parent}
{
    setWindowTitle(tr("PCRE Regular Expression Cheatsheet"));

    auto content = new QTextEdit{get_pcre_cheatsheet_content()};
    content->setReadOnly(true);

    auto button_box = new QDialogButtonBox{QDialogButtonBox::Close};

    auto layout = new QVBoxLayout;
    layout->addWidget(content);
    layout->addWidget(button_box);

    setLayout(layout);
    resize(1200, 600);

    connect(button_box, &QDialogButtonBox::rejected, this, &QDialog::accept);
}
} // namespace flan
