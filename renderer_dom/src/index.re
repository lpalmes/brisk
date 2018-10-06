open ReactDomComponents;

module Component = {
  let otherComponent = React.reducerComponent("Other");
  let createElement = (~children as _, ()) => {
    ...otherComponent,
    initialState: _ => false,
    reducer: (x, _) => React.Update(x),
    render: ({state, reduce}) =>
      if (state) {
        <View>
          <Button title="Cell one" callback={reduce(() => !state)} />
          <Button title="Cell two" callback={reduce(() => !state)} />
        </View>;
      } else {
        <View> <Button title="Well" callback={reduce(() => !state)} /> </View>;
      },
  };
};

switch (Webapi.Dom.Document.getElementById("index", Webapi.Dom.document)) {
| Some(container) =>
  React.RunLoop.run(container, React.element(<Component />))
| None => print_endline("Couldn't find node")
};