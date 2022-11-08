#include <qtmdns/abstractserver.hpp>

using namespace QtMdns;

AbstractServer::AbstractServer(QObject* parent)
    : QObject(parent)
{}

AbstractServer::~AbstractServer()
{
}
