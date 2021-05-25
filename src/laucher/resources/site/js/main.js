function initialize() {
    loadNavigationPage("s1x");
}

function loadNavigationPage(page) {
    var content = document.querySelector("#content");
    fetch(`./pages/${page}.html`).then(data => {
        return data.text()
    }).then(text => {
        content.innerHTML = text;
    });
}

window.addEventListener("load", initialize);