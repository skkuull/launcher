window.addEventListener("load", initialize);

function sleep(milliseconds) {
    return new Promise(resolve => {
        setTimeout(resolve, milliseconds);
    });
}

function makeSleep(milliseconds) {
    return () => sleep(milliseconds);
}

function waitForAllImages() {
    return new Promise(resolve => {
        function waitForAllImagesInternal() {
            const images = document.querySelectorAll('img');

            for (var i = 0; i < images.length; ++i) {
                if (!images[i].complete) {
                    window.requestAnimationFrame(waitForAllImagesInternal);
                    return;
                }
            }

            resolve();
        }

        waitForAllImagesInternal();
    });
}

function initialize() {
    initializeNavigation() //
        .then(() => waitForAllImages()) //
        .then(makeSleep(300))
        .then(() => window.executeCommand("show"));

    document.querySelector("#minimize-button").onclick = () => {
        window.executeCommand("minimize");
    };

    document.querySelector("#close-button").onclick = () => {
        window.executeCommand("close");
    };
}

function initializeNavigation() {
    var elements = document.querySelectorAll("#navigation>.element");
    elements.forEach(e => {
        e.addEventListener("click", handleNavigationClick);
    });

    return loadInitialPage();
}

function removeActiveElement() {
    var element = document.querySelector("#navigation>.element.active");
    if (element) {
        element.classList.remove("active");
    }
}

function handleNavigationClick(e) {
    const el = e.target;
    if (el.classList.contains("active")) {
        return;
    }

    removeActiveElement();
    el.classList.add("active");
    loadNavigationPage(el.id);
}

function loadInitialPage() {
    const el = document.querySelector("#navigation>.element.active");
    return loadNavigationPage(el.id);
}

function loadNavigationPage(page) {
    var content = document.querySelector("#content");
    return fetch(`./pages/${page}.html`).then(data => {
        return data.text()
    }).then(text => {
        content.innerHTML = text;
    });
}