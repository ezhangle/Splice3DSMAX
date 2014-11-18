macroScript SpliceMaxRendering 
enabledIn:#("max")
category:"SpliceMax"
internalCategory:"SpliceMax"
tooltip:"Enable Splice Rendering"
buttontext:"Rendering"
(
    on execute do  Splice.Rendering = not Splice.Rendering
    on isChecked return Splice.Rendering
)


macroScript SpliceMaxManipulation
enabledIn:#("max")
category:"SpliceMax"
internalCategory:"SpliceMax"
tooltip:"Enable Splice Manipulation"
buttontext:"Manipulation"
(
    on execute do  Splice.Manipulation = not Splice.Manipulation
    on isChecked return Splice.Manipulation
)
