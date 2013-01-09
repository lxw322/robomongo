#include "robomongo/core/domain/MongoServer.h"
#include "robomongo/core/domain/MongoDatabase.h"
#include "robomongo/core/settings/ConnectionSettings.h"
#include "robomongo/core/AppRegistry.h"
#include "robomongo/core/EventBus.h"
#include "robomongo/gui/widgets/explorer/ExplorerServerTreeItem.h"
#include "robomongo/gui/widgets/explorer/ExplorerDatabaseTreeItem.h"
#include "robomongo/gui/GuiRegistry.h"

using namespace Robomongo;

ExplorerServerTreeItem::ExplorerServerTreeItem(MongoServer *server) : QObject(),
    _server(server),
    _bus(AppRegistry::instance().bus())
{
    _bus->subscribe(this, DatabaseListLoadedEvent::Type, _server);

    setText(0, _server->connectionRecord()->getReadableName());
    setIcon(0, GuiRegistry::instance().serverIcon());
	setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);
}

ExplorerServerTreeItem::~ExplorerServerTreeItem()
{
    int z = 56;
}

void ExplorerServerTreeItem::expand()
{
    _server->loadDatabases();
}

void ExplorerServerTreeItem::databaseRefreshed(const QList<MongoDatabase *> &dbs)
{
    // Remove child items
	int itemCount = childCount();
	for (int i = 0; i < itemCount; ++i)
	{
		QTreeWidgetItem * p = child(0);
		removeChild(p);
		delete p;
	}

    // Add 'System' folder
    QIcon folderIcon = GuiRegistry::instance().folderIcon();
    QTreeWidgetItem * systemFolder = new QTreeWidgetItem();
    systemFolder->setIcon(0, folderIcon);
    systemFolder->setText(0, "System");
    addChild(systemFolder);

    for (int i = 0; i < dbs.size(); i++)
    {
        MongoDatabase *database = dbs.at(i);

        if (database->isSystem()) {
            ExplorerDatabaseTreeItem * dbItem = new ExplorerDatabaseTreeItem(database);
            systemFolder->addChild(dbItem);
            continue;
        }

        ExplorerDatabaseTreeItem * dbItem = new ExplorerDatabaseTreeItem(database);
        addChild(dbItem);
    }

    // Show 'System' folder only if it has items
    systemFolder->setHidden(systemFolder->childCount() == 0);
}

void ExplorerServerTreeItem::handle(DatabaseListLoadedEvent *event)
{
    databaseRefreshed(event->list);
}