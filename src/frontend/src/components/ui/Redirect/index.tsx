import { Component } from 'preact';
import { route } from 'preact-router';

type RedirectProps = {
  to: string;
};
export default class Redirect extends Component<RedirectProps> {
  componentWillMount() {
    route(this.props?.to, true);
  }

  render() {
    return null;
  }
}
