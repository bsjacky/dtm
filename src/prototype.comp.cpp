#include <prototype/components/metamoduleflood.comp.h>
#include <prototype/components/metamoduleviewer.comp.h>
#include <prototype/components/prototype.comp.h>

#include <flowvr/app/components/connection.comp.h>
#include <flowvr/app/core/genclass.h>

using namespace flowvr;

namespace prototype
{

GENCLASS(prototype)

void prototype::execute() {
  Component* viewer = addObject(MetaModuleViewer("viewer"));
  Component* flood = addObject(MetaModuleFlood("flood"));

  // connexion du module d'inondation au module de visualisation
  Component* floodToViewer = addObject(Connection("floodToViewer"));
  link(*(flood->getPort("waterOut")), *(floodToViewer->getPort("in")));
  link(*(floodToViewer->getPort("out")), *(viewer->getPort("waterIn")));

  // connexion du module de visualisation au module d'inondation
  Component* viewerToFlood = addObject(Connection("viewerToFlood"));
  link(*(viewer->getPort("dtmOut")), *(viewerToFlood->getPort("in")));
  link(*(viewerToFlood->getPort("out")), *(flood->getPort("dtmIn")));
}

}
