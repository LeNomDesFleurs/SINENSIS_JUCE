# ajouter un listener pour modifier l'UI

[doc de JUCE à ce propos](https://docs.juce.com/master/tutorial_listeners_and_broadcasters.html)

comprends pas bien la dif entre main component et editor, je vais essayer d'appliquer le tuto à l'editeur

juce::AudioProcessorEditor hérite de juce component en public, le redire crée de l'ambiguité et génère des erreurs, c'est bon à savoir

`override` n'est utilisé qu'à la declaration, pas à la définition.

Ajouter l'héritage de la classe listener de l'objet en question à l'éditeur

`: public juce::Button::Listener `

Ajouter l'éditeur comme listener (ici dans le constructeur)
`checkTheTimeButton.addListener (this);`

override la fonction de callback

```
void buttonClicked (juce::Button* button) override // [2]
{
}
```

vérifier à quel bouton ça correspond :

```
if (button == &checkTheTimeButton)
{ do stuff }

```
