# starscape
Use a contextual markov model to generate novel fictional status updates or something

Boils down to *press a button and a never before seen, entirely machine generated article emerges from stdout*

Use a small recursive token substitution routine to generate an 'article header', append that to a context corpus containing a small essay about the desired subject. (In this case, the 2019 anime 'startwinkle precure') and submit this into a text generating blackbox.

For my examples I use gpt-2 from openai using the 800 million paramter  model.

This is fed into ./newspaper.sh which randomly creates an article page. Using a random header, paper texture, font, and layout.

![](https://github.com/keptan/starscape/raw/master/examples.png)
