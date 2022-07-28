
#pragma once

#include <QDialog>
#include <QObject>
#include <QString>

namespace flan
{
class data_source_t;

class data_source_delegate_t : public QObject
{
    Q_OBJECT

public:
    data_source_delegate_t(QString name, data_source_t& data_source, QObject* parent = nullptr);

    QString name() const { return _name; }
    QString info() const;
    QString error_message() const;

    data_source_t& data_source() { return _data_source; }

    virtual bool is_settings_supported() const = 0;
    virtual QDialog* settings_dialog(QWidget* parent = nullptr) = 0;

signals:
    void info_changed(QString info);

    //! Emitted every time an error occured with the given \a error_message.
    //!
    //! If the \a error_message is null (QString::isNull()) the error has been cleared.
    void error_changed(QString error_message);

private:
    QString _name;
    data_source_t& _data_source;
};
} // namespace flan
