Qt Animation Framework v1.0

This component adds a convenient API to do animations in Qt. Note
that this solution is experimental and work in progress: No
backwards compatibility (source nor binary) is provided.

The Animation Framework lets you define, run and control
animations, states and transitions. The foundation classes
QtAnimation and QtAnimationGroup are used to scoreboard both
simple and complex animations. A simple animation might change a
widget's geometry from one rectangle to another, over the course
of 2 seconds, using a linear easing curve. You can daisy-chain
animations by adding them to a sequential QtAnimationGroup, a
class that also gives you fine grained control over pauses and
combinations of parallel and sequential subanimations.

You can also define properties for your objects in states using
QtState, add states to a QtStateGroup to manage exclusivity for
your states, and define animated transitions between the states
using QtTransition. The Animation Framework classes help maintain
the necessary bookkeeping in order to provide animated forms and
scenes.

