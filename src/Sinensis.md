# Sinensis

-   [ ] Frequency widget
-   [ ] Q widget
-   [ ] Ratio widget
-   [ ] Band widget
-   [ ] Buffer le background
-   [x] ~~use an enum instead of multiple true false for mode~~
-   [x] ~~Clean the ui header mess~~
-   [ ] replacer tout les widgets dans les bounds
-   [ ] charte d'épaisseur des traits ?

## Remplacer les modes par des Enum

Un peu relou pour le lien GUI / APVTS
Pour le type de paramètre, on a `AudioParameterChoice` qui est fait pout ça

```cpp
params.push_back(std::make_unique<AudioParameterChoice> ("BANDMODE", "Band Mode", StringArray { "Low / High", "Odd / Even", "Peak" }, 0));
```

La question c'est quel attachment utiliser dans le `Editor`.

Sur [ce thread](https://forum.juce.com/t/mapping-a-audioparameterchoice-to-a-radio-button-group-sensecheck/18760) ça parle de mapper des radioButtons sur un `AudioParameterChoice` sauf que personne lui a répondu et que je comprends pas bien ce qu'il essaie de faire

Sur [ce thread](https://forum.juce.com/t/combobox-and-audioparameterchoice/31332) du forum ça recommande de faire son propre attachment.
[Ce thread](https://forum.juce.com/t/how-to-implement-radio-buttons-with-parameterattachment-right/44747/3) semble donner un exemple d'attachment custom, j'ai copié son header et cpp pour voir à quoi ça ressemble

## Q widget

-   [ ] le peak coupe en haut et les courbe coupe en bas, ajouter des marge en fonction de l'épaisseur

Assez simple, deux courbe quatratic qui se font face avec les points de contrôle qui monte moins vite que le peak

## Ratio widget

Placer un centre, ajouter ou soustraire en fonction du ratio

## Band Widget

-   [ ] offset, ne se remplit jamais complètement
-   [x] ~~Utiliser un gradient sur les sliders en fonction du type de mode~~
-   [x] ~~plus simple -> couleur en fontion du gain, trouver une interpolation qui fait sens (rouge -> vert fait un dégradé grisouille moche)~~
-   [x] ~~O|E instead of Odd Even and L|H to save size ?~~

Obligé de le faire sur le resize ? J'ai besoin de la valeur du bandMode...

## Frequency cutoff Widget

-   [ ] Root Freq
-   [ ] Other bands freq and gain
-   [ ] Midi Mono Root freq
-   [ ] Midi Poly Root Freq

Pareil, je vais probablement devoir le faire en paint, faire un slider vide sur le visuel ?
Donc potentiellement Band et Frequency seraient juste des slider vide

## Bug Editor constructor not recognized

Tout à l'heure c'était un problème de fonction pas implémentée
je viens de renommer les extensions d'header, y'a bien moyen que ça vienne de là
Bon visiblement il fallait appeler le header du `editor` dans le cpp de `processor` pour que la classe sois déjà déclarer, j'avoue que ça me semble pas hyper safe ces histoires d'entre header call

## On making Path

Pour dessiner la plupart des choses, il faut

-   instantier un nouveau path
-   le remplir avec des trucs
-   le stroke dans un objet graphic

Dans le cas d'une ligne :

```cpp
juce::Path Lines;
juce::Point<float> point1, point2;
lines.startNewSubPath(point1);
lines.lineTo(point2); //Pas possible de faire les deux d'un coup ??
g.setColout(juce::Colours::black);
g.strokePath(lines, {width, PathStrokeType::curved, PathStrokeType::rounded});
```

Pour faire un rounded Rectangle

```cpp
juce::Path rounded_rectangle;
rounded_rectangle.addRoundedRectangle(
      juce::Rectangle<float>(20, 150, 100, 200), 3);
//stroke ...
```

Beaucoup plus simple :

````cpp
g.fillRoundedRectangle(x, y, width, height, corner size);
```

## Paint background

problème : je ne peux pas draw ma fonction à chaque repaint car elle inclut du random, et puis ce serait pas très malin d'un point de vu processeur, dont besoin de trouver un moyen de paint le background une seul fois.

Avec des borne qui ne s'active que la première fois, ça ne marche pas, je suppose que JUCE flush tout le contenu entre deux frames

[Ce thread](https://forum.juce.com/t/ive-asked-this-before-but-nobody-seemed-to-know-the-answer-i-have-a-component-that-draws-a-vertical-line-playhead-but-it-calls-repaint-on-the-parent-i-just-want-to-draw-the-line/31913/2) mentionne `setBufferToImage()`, [documentation ici](https://docs.juce.com/develop/classComponent.html#af19bbc2186e3297ddd55c328e46c014b).

De ce qu'il me semble comprendre, il faut faire un component et appeler sur lui `setBufferedToImage(true)`.
````
