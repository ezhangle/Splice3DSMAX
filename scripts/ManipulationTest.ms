
m = SpliceMesh()

entryPt = "eventLogger";
klCode = "
require Manipulation;

object EventLogger : Manipulator {
};

function EventLogger.onEvent!(io Event event){
    report(\"EventLogger.onEvent:\" + event.eventType());
}

function EventLogger.onEnable!(){
    report(\"EventLogger.onEnable\");
}

function EventLogger.onDisable!(){
    report(\"EventLogger.onDisable\" );
}

operator eventLogger() {

  // Setup the manipulator
  Ref<EventDispatcher> eventDispatcher = EventDispatcher_GetInstance();
  report(\"eventLogger:\" + eventDispatcher.activeManipulator());
  if(eventDispatcher.activeManipulator() != 'EventLogger')
  {
      eventDispatcher.registerManipulator(EventLogger());

      // activate the manipulator
      eventDispatcher.pushManipulator('EventLogger');
  }
}
"
m.SetKLCode entryPt klCode


Splice.manipulation = true