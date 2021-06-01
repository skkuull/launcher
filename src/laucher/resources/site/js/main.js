function initialize() {
    initializeNavigation();
}

function initializeNavigation() {
    var elements = document.querySelectorAll("#navigation>.element");
    elements.forEach(e => {
        e.addEventListener("click", handleNavigationClick);
    });

    loadInitialPage();
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
    loadNavigationPage(el.id);
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